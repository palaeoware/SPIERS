#ifndef STATICFUNCTIONS_H
#define STATICFUNCTIONS_H

/**
 * @brief The StaticFunctions class
 */
class StaticFunctions
{
public:
    static void transposeMatrix(float *matrix);
    static void invertEndian(unsigned char *data, int count);
};

#endif // STATICFUNCTIONS_H
