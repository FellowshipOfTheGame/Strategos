#ifndef ALGORITHM_H
#define ALGORITHM_H

// Classe base para Algoritmo Genetico

class Algorithm
{
    private:

    public:
        Algorithm();
        virtual ~Algorithm();

        virtual void initialize() = 0;
        virtual void run() = 0;
};

#endif // ALGORITHM_H
