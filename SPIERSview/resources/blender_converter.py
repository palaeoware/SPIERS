import bpy
import sys
import json

if len(sys.argv) < 4:
    print("Error: Missing arguments")
    sys.exit(1)

fbx_path = sys.argv[-3]
materials_json = sys.argv[-2]
blend_output = sys.argv[-1]

try:
    # Clear default scene
    bpy.ops.object.select_all(action='SELECT')
    bpy.ops.object.delete(use_global=False)

    # Import FBX
    bpy.ops.import_scene.fbx(filepath=fbx_path)

    # Load materials from JSON
    try:
        materials = json.loads(materials_json)
    except:
        materials = {}

    # Apply materials to objects
    for obj_name, mat_data in materials.items():
        obj = bpy.data.objects.get(obj_name)
        if obj and obj.type == 'MESH':
            # Create material
            mat = bpy.data.materials.new(name=obj_name + "_Mat")
            mat.use_nodes = True

            # Set BSDF properties
            bsdf = mat.node_tree.nodes.get("Principled BSDF")
            if bsdf:
                # Color with alpha
                color = mat_data.get("color", [1.0, 1.0, 1.0])
                alpha = mat_data.get("alpha", 1.0)

                # Normalize color into exactly 3 floats
                if not isinstance(color, (list, tuple)):
                    color = [1.0, 1.0, 1.0]
                elif len(color) < 3:
                    color = [1.0, 1.0, 1.0]
                else:
                    color = [float(color[0]), float(color[1]), float(color[2])]

                alpha = float(alpha)

                bsdf.inputs["Base Color"].default_value = (color[0], color[1], color[2], alpha)
                bsdf.inputs["Metallic"].default_value = float(mat_data.get("metallic", 0.0))
                bsdf.inputs["Roughness"].default_value = float(mat_data.get("roughness", 0.5))

            # Assign material to object
            if len(obj.data.materials) == 0:
                obj.data.materials.append(mat)
            else:
                obj.data.materials[0] = mat

    # Save blend file
    bpy.ops.wm.save_as_mainfile(filepath=blend_output)

except Exception as e:
    print(f"Error: {e}")
    sys.exit(1)

sys.exit(0)