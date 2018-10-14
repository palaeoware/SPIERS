#ifndef SCALARFIELDLAYER_H
#define SCALARFIELDLAYER_H

/**
 * @brief The ScalarFieldLayer class
 */
class ScalarFieldLayer
{
public:
    int *edges;  /* tag indexing into vertex list */
    unsigned char *botData;
    unsigned char *topData;

    ScalarFieldLayer();
};

#endif // SCALARFIELDLAYER_H
