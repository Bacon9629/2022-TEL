#include <Arduino.h>
#include <Vector.h>
#include "Learning.h"
using namespace std;

void setup() {
//    ALayer aLayer = ALayer(3, 1, new Sigmoid);

//    Vector<Vector<double>> temp_x = {{0, 1}, {1, 1}};
//    Vector<Vector<double>> temp_target = {{0}, {1}};

    Vector<Vector<double>> temp_x = {{0, 0, 1}, {0, 1, 1}, {1, 0, 1}, {1, 1, 1}};
    Vector<Vector<double>> temp_target = {{0}, {1}, {1}, {0}};
//    Vector<Vector<double>> temp_target = {{0, 1}, {0, 1}, {1, 0}, {1, 0}};

    // data init
    MyMatrix x = MyMatrix(temp_x);
    MyMatrix target = MyMatrix(temp_target);

    // active func
    Sigmoid sigmoid = Sigmoid();
    Tanh tanh = Tanh();

    // define network
    /**
     * loss function: MSE
     * */
//    MyFrame frame = MyFrame(new MSE, -1);

    /**
     * loss function: cross entropy
     * */
    MyFrame frame = MyFrame(new CrossEntropy, -1);


    /**
     * active function: sigmoid
     * optimizer: MMT
     * */
//    frame.add(new DenseLayer(3, 5, &sigmoid, new MMT(0.9)));
//    frame.add(new DenseLayer(5, 1, &sigmoid, new MMT(0.9)));


    /**
     * active function: sigmoid
     * optimizer: XOR
     * */
    frame.add(new DenseLayer(3, 5, &sigmoid, new XOR(0.9)));
    frame.add(new DenseLayer(5, 1, &sigmoid, new XOR(0.9)));
    
    // frame.add(new DenseLayer(3, 128, &sigmoid, new XOR(0.9)));
    // frame.add(new DenseLayer(128, 128, &sigmoid, new XOR(0.9)));
    // frame.add(new DenseLayer(128, 1, &sigmoid, new XOR(0.9)));


    /**
     * active function: tanh
     * optimizer: XOR
     * */
//    frame.add(new DenseLayer(3, 5, &tanh, new XOR(0.9)));
//    frame.add(new DenseLayer(5, 1, &tanh, new XOR(0.9)));

    frame.train(4000, x, target);

    frame.show(x, target);
    // return 0;
}


void loop(){

}