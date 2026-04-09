# FBX Exporter Structural Bug Fix

## The Bug

**Root Cause:** All top-level sibling nodes were being written *into* a `documentBuffer` that was itself written as the content of a `Document` node. This created an incorrect hierarchy where:

```
Document (node)
  └─ FBXHeaderExtension (incorrectly nested as Document's child)
  └─ RootNode (incorrectly nested)
  └─ Takes (incorrectly nested)
  └─ null record
```

**Why it happened:** The code was:
1. Creating a `QByteArray documentBuffer`
2. Writing all top-level nodes (FBXHeaderExtension, RootNode, Takes) into this buffer
3. Then writing the buffer as the content of a single Document node

This made the first-level siblings into children of Document, which is structurally invalid in FBX 7.3.

## The Fix

**New Structure:** Top-level sibling nodes are now written **directly to `outFile`** as true siblings, in correct FBX 7.3 order:

```
FBXHeaderExtension (top-level sibling)
  └─ null record (terminates child list)
Documents (top-level sibling)
  └─ null record
Definitions (top-level sibling)
  └─ null record
Objects (top-level sibling)
  ├─ Geometry node 1 (child)
  │  └─ null record
  ├─ Geometry node 2 (child)
  │  └─ null record
  └─ null record (terminates Objects' child list)
Connections (top-level sibling)
  └─ null record
[top-level null record] (terminates list of siblings)
```

### Key Changes

1. **Helper Functions Added:**
   - `writeNullRecord()` - writes 13-byte null child terminator
   - `encodeInt32Property()` - encodes 'I' type + 4-byte int32
   - `encodeInt64Property()` - encodes 'L' type + 8-byte int64
   - `encodeFloatArrayProperty()` - encodes 'f' type + array metadata + float data
   - `encodeInt32ArrayProperty()` - encodes 'i' type + array metadata + int32 data

2. **Node Writing:**
   - Each top-level node (FBXHeaderExtension, Documents, etc.) is written directly to `outFile`
   - Each node calculates its own `endOffset` = header_size + property_bytes + 13 (null terminator)
   - Each node writes its 13-byte null child terminator
   - Sibling nodes are NOT wrapped in a parent container

3. **Objects Node Structure:**
   - Objects is a top-level node that contains geometry nodes as children
   - Each geometry node has:
     - endOffset pointing past its null terminator
     - One property: object ID (int64)
     - Array properties: vertices (float array), polygon indices (int array), normals (float array)
     - 13-byte null child terminator
   - Objects itself ends with a 13-byte null terminator
   - This structure is correct per FBX 7.3 spec

4. **Top-Level Termination:**
   - After Connections node, a final 13-byte null record is written
   - This terminates the list of top-level sibling nodes

## Binary Layout (Correct)

Each node in FBX 7.3 follows:
```
Offset  Size  Field
0       4     EndOffset (uint32) - points past null terminator of this node
4       4     NumProperties (uint32)
8       4     PropertyListLen (uint32)
12      1     NameLen (uint8)
13      N     Name (N bytes)
13+N    M     Property data (M bytes, determined by PropertyListLen)
13+N+M  13    Null record (child list terminator)
```

`EndOffset = 13 + N + M + 13` where:
- 13 = fixed header before name
- N = name length
- M = property data length
- 13 = null child terminator

## Verification

The FBX file now has:
- ✓ Correct header (magic + version)
- ✓ Top-level sibling nodes with proper endOffsets
- ✓ Each node properly terminated with 13-byte null record
- ✓ Objects contains geometry as children (not siblings of Documents)
- ✓ No nodes accidentally nested inside other nodes' payloads
- ✓ Geometry data encoded with proper FBX property type codes

The file should now be importable by Blender, Maya, and other FBX 7.3 readers.
