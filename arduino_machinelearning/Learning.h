#ifndef _LEARNING_H_
#define _LEARNING_H_

// #include <iostream>
#include <time.h>
// #include <utility>
#include <Vector.h>
#include <math.h>

using namespace std;

class MyMatrix{
public:
    Vector<Vector<double>> matrix;

    MyMatrix() {
        init(0, 0, 0);
    }

    MyMatrix(Vector<Vector<double>> _Vector){
        init(_Vector);
    }

    MyMatrix(size_t row, size_t col, double init_val){
        init(row, col, init_val);
    }

    inline static size_t row(MyMatrix *_matrix){
        return _matrix->matrix.size();
    }

    inline static size_t col(MyMatrix *_matrix){
        return _matrix->matrix[0].size();
    }

    // 取 start 到 end - 1 的row
    inline static MyMatrix getRow(MyMatrix &_matrix, size_t start, size_t end){
        if (end > _matrix.row() || start < 0){
            // cout << "shape_wrong: MyMatrix getRow, start: " << start
            // << "  , end: " << end << endl;
        }
        Vector<Vector<double>> result(_matrix.matrix.begin() + start, _matrix.matrix.begin() + end);
        return MyMatrix(result);
    }

    static MyMatrix dot(MyMatrix *matrix_a, MyMatrix *matrix_b){
        size_t row_a = MyMatrix::row(matrix_a);
        size_t col_a = MyMatrix::col(matrix_a);
        size_t row_b = MyMatrix::row(matrix_b);
        size_t col_b = MyMatrix::col(matrix_b);

        if (col_a != row_b){
            // std:: cout << "shape wrong" << std::endl;
        }

        const size_t row_result = row_a;
        const size_t col_result = col_b;

        Vector<Vector<double>> result(row_result, Vector<double>(col_result));

        for (int r = 0; r < row_result; r++) {
            for (int c = 0; c < col_result; c++) {
                // 指定在result內的哪個位置
                // 接下來依照指定的result位置取出a、b的值來做計算

                for (int i = 0; i < col_a; i++) {
                    result[r][c] += matrix_a->matrix[r][i] * matrix_b->matrix[i][c];
                }

            }
        }

        MyMatrix temp(result);
        return temp;
    }

    inline static MyMatrix transpose(MyMatrix *_matrix) {
        Vector<Vector<double>> result(_matrix->col(), Vector<double>(_matrix->row()));

        for (size_t i = 0; i < _matrix->row(); i++){
            for (size_t j = 0; j < _matrix->col(); j++){
                result[j][i] = _matrix->matrix[i][j];
            }
        }

        MyMatrix temp(result);
        return temp;
    }

    inline static MyMatrix expand_row(MyMatrix *matrix_a, MyMatrix *matrix_b){
        MyMatrix _temp_b;
//        if(matrix_a->row() != matrix_b->row()){
        _temp_b = MyMatrix(matrix_a->row(), matrix_b->col(), 0);
        for (int i=0;i<matrix_a->row();i++){
            _temp_b.matrix[i] = matrix_b->matrix[0];
        }
//        }
        return _temp_b;
    }

    static MyMatrix add(MyMatrix *matrix_a, MyMatrix *matrix_b){
        MyMatrix _temp_b;
        if(matrix_a->row() != matrix_b->row()){
            _temp_b = expand_row(matrix_a, matrix_b);
            matrix_b = &_temp_b;
        }

        if (matrix_a->row() != matrix_b->row() || matrix_a->col() != matrix_b->col()){
            //std:: cout << "shape wrong" << std::endl;
        }

        MyMatrix result(matrix_a->row(), matrix_b->col(), 0);
        for (int i=0;i<result.row();i++){
            for (int j=0;j<result.col();j++){
                result.matrix[i][j] = matrix_a->matrix[i][j] + matrix_b->matrix[i][j];
            }
        }
        return result;
    }

    inline static MyMatrix add(MyMatrix *matrix, double val){
        MyMatrix val_matrix(matrix->row(), matrix->col(), val);
        MyMatrix result_matrix(MyMatrix::add(matrix, &val_matrix));
        return result_matrix;
    }

    static MyMatrix reduce(MyMatrix *matrix_a, MyMatrix *matrix_b){
        MyMatrix _temp_b;
        if(matrix_a->row() != matrix_b->row()){
            _temp_b = expand_row(matrix_a, matrix_b);
            matrix_b = &_temp_b;
        }

        if (matrix_a->row() != matrix_b->row() || matrix_a->col() != matrix_b->col()){
            // std:: cout << "shape wrong" << std::endl;
        }
        MyMatrix result(matrix_a->row(), matrix_b->col(), 0);
        for (int i=0;i<result.row();i++){
            for (int j=0;j<result.col();j++){
                result.matrix[i][j] = matrix_a->matrix[i][j] - matrix_b->matrix[i][j];
            }
        }
        return result;
    }

    inline static MyMatrix reduce(MyMatrix *matrix, double val){
        MyMatrix val_matrix(matrix->row(), matrix->col(), val);
        MyMatrix result_matrix(MyMatrix::reduce(matrix, &val_matrix));
        return result_matrix;
    }

    inline static MyMatrix times(MyMatrix *matrix_a, MyMatrix *matrix_b){
        MyMatrix _temp_b;
        if(matrix_a->row() != matrix_b->row()){
            _temp_b = expand_row(matrix_a, matrix_b);
            matrix_b = &_temp_b;
        }

        MyMatrix result(matrix_a->row(), matrix_a->col(), 0);
        for (int i=0;i<result.row();i++){
            for (int j=0;j<result.col();j++){
                result.matrix[i][j] = matrix_a->matrix[i][j] * matrix_b->matrix[i][j];
            }
        }
        return result;
    }

    inline static MyMatrix times(MyMatrix *matrix, double val){
        MyMatrix val_matrix(matrix->row(), matrix->col(), val);
        MyMatrix result_matrix(MyMatrix::times(matrix, &val_matrix));
        return result_matrix;
    }

    void init(Vector<Vector<double>> _Vector){
        this->matrix = {_Vector.begin(), _Vector.end()};
    }

    void init(size_t row, size_t col, double init_val){
        Vector<Vector<double>> _Vector = Vector<Vector<double>>(row, Vector<double>(col, init_val));
        init(_Vector);
    }

    inline size_t row() {
        return MyMatrix::row(this);
    }

    inline size_t col() {
        return MyMatrix::col(this);
    }

    inline MyMatrix dot(MyMatrix *matrix_b) {
        return MyMatrix::dot(this, matrix_b);
    }

    inline MyMatrix transpose() {
        return MyMatrix::transpose(this);
    }

    inline MyMatrix add(MyMatrix *_matrix) {
        return MyMatrix::add(this, _matrix);
    }

    inline MyMatrix add(double val) {
        return MyMatrix::add(this, val);
    }

    inline MyMatrix reduce(MyMatrix *_matrix) {
        return MyMatrix::reduce(this, _matrix);
    }

    inline MyMatrix reduce(double val) {
        return MyMatrix::reduce(this, val);
    }

    inline MyMatrix multiplication(MyMatrix *_matrix) {
        return MyMatrix::times(this, _matrix);
    }

    inline MyMatrix multiplication(double val) {
        return MyMatrix::times(this, val);
    }

    inline void random_matrix(){
        srand(time(NULL));
        for (int i=0;i<this->row();i++){
            for (int j=0;j<this->col();j++){
                matrix[i][j] = rand() / (RAND_MAX + 1.0) - 0.5;
            }
        }
    }

    inline void print_matrix(){
        for (int i=0;i<this->row();i++){
            for (int j=0;j<this->col();j++){
                // // cout << matrix[i][j] << ", ";
            }
            // // cout << endl;
        }
    }

    inline void print_shape(){
        // // cout << "rol: " << row() << " col: " << col() << endl;
    }

};


// loss function - start

class LossFunc{
public:
    virtual double forward(MyMatrix &y, MyMatrix &target) = 0;
    virtual MyMatrix backward(MyMatrix &y, MyMatrix &target) = 0;
};

class MSE: public LossFunc{
public:
    double forward(MyMatrix &y, MyMatrix &target) override{
        double result = 0;
        MyMatrix temp = MyMatrix::reduce(&y, &target);
        temp = MyMatrix::times(&temp, &temp);

        for(size_t r=0;r<y.row();++r)
            for(size_t c=0;c<y.col();++c)
                result += temp.matrix[r][c];

        result /= y.row() * y.col();
        return result;
    }

    MyMatrix backward(MyMatrix &y, MyMatrix &target) override {
        return MyMatrix::reduce(&y, &target);
    }
};

class CrossEntropy: public LossFunc{
public:
    double forward(MyMatrix &y, MyMatrix &target) override {
        double result = 0;
        for (size_t i = 0; i< y.row(); i++){
            for (size_t j = 0; j < y.col(); j++){
                double _y = y.matrix[i][j];
                double _target = target.matrix[i][j];
                result += -_target * log(_y) - (1 - _target) * log(1 - _y);
            }
        }
        return result;
    }

    MyMatrix backward(MyMatrix &y, MyMatrix &target) override {
        MyMatrix result(y.row(), y.col(), 0);

        for (size_t i = 0; i< y.row(); i++){
            for (size_t j = 0; j < y.col(); j++){
                double _y = y.matrix[i][j];
                double _target = target.matrix[i][j];
                result.matrix[i][j] = (_y - _target) / (_y * (1 - _y));
            }
        }

        return result;
    }
};

// loss function - end


// active function - start

class ActiveFunc{
public:
    virtual MyMatrix func_forward(MyMatrix x) = 0;
    virtual MyMatrix func_backward(MyMatrix x) = 0;

};

class Sigmoid: public ActiveFunc{
public:
    MyMatrix func_forward(MyMatrix x) override {
        MyMatrix result(x.row(), x.col(), 0);

        for (int i = 0; i< x.row(); i++){
            for (int j = 0; j< x.col(); j++) {
                result.matrix[i][j] = 1 / (1 + exp(-x.matrix[i][j]));

            }
        }
        return result;
    }

    MyMatrix func_backward(MyMatrix x) override {
        MyMatrix a = func_forward(x);
        MyMatrix b = MyMatrix::reduce(&a, 1);
        b = MyMatrix::times(&b, -1);
        return MyMatrix::times(&a, &b);
    }
};

class Tanh:public ActiveFunc{
public:
    MyMatrix func_forward(MyMatrix x) override {
        MyMatrix result(x.row(), x.col(), 0);

        for (int i = 0; i< x.row(); i++){
            for (int j = 0; j< x.col(); j++) {
                double a = exp(x.matrix[i][j]);
                double b = exp(-x.matrix[i][j]);
//                double c = (a - b) / (b - a);
                result.matrix[i][j] = (a - b) / (b - a);
            }
        }
        return result;
    }

    MyMatrix func_backward(MyMatrix x) override {
        MyMatrix result(x.row(), x.col(), 0);

        for (int i = 0; i< x.row(); i++){
            for (int j = 0; j< x.col(); j++) {
                double a = exp(x.matrix[i][j]);
                double b = exp(-x.matrix[i][j]);
                double c = (a - b) / (b - a);
                result.matrix[i][j] = 1 - c * c;
            }
        }


        return result;
    }

};


// active function - end


// Optimizer - start

class Optimizer{
public:
    virtual void gradient_decent(MyMatrix &w, MyMatrix &b, MyMatrix &grad_w, MyMatrix &grad_b) = 0;
};

class XOR: public Optimizer{
public:
    double eta;

    XOR(double _eta){
        eta = _eta;
    }

    void gradient_decent(MyMatrix &w, MyMatrix &b, MyMatrix &grad_w, MyMatrix &grad_b) override {
        double _temp = eta / w.row();

        MyMatrix temp_w = MyMatrix::times(&grad_w, _temp);
        w = MyMatrix::reduce(&w, &temp_w);

        MyMatrix temp_b = MyMatrix::times(&grad_b, _temp);
        b = MyMatrix::reduce(&b, &temp_b);
    }

};

class MMT: public Optimizer{
public:
    double eta;
    double beta = 0.9;
    MyMatrix last_grad_w = MyMatrix();
    MyMatrix last_grad_b = MyMatrix();

    MMT(double _eta){
        init(_eta, 0.9);
    }

    MMT(double _eta, double _beta){
        init(_eta, _beta);
    }

    void init(double _eta, double _beta){
        eta = _eta;
        beta = _beta;
    }

    void gradient_decent(MyMatrix &w, MyMatrix &b, MyMatrix &grad_w, MyMatrix &grad_b) override {
//        last_grad_w =  alpha * grad_w + beta * last_grad_w;
//        w -= last_grad_w;
        if (last_grad_w.row() == 0){
            last_grad_w = MyMatrix(grad_w.row(), grad_w.col(), 0);
            last_grad_b = MyMatrix(grad_b.row(), grad_b.col(), 0);
        }

        MyMatrix temp_a = MyMatrix::times(&grad_w, eta);
        MyMatrix temp_b = MyMatrix::times(&last_grad_w, beta);
        last_grad_w = MyMatrix::add(&temp_a, &temp_b);
        w = MyMatrix::reduce(&w, &last_grad_w);

        temp_a = MyMatrix::times(&grad_b, eta);
        temp_b = MyMatrix::times(&last_grad_b, beta);
        last_grad_b = MyMatrix::add(&temp_a, &temp_b);
        b = MyMatrix::reduce(&b, &last_grad_b);

    }

};

// Optimizer - end


class Layer{
public:
    MyMatrix x;  // 輸入
    MyMatrix y;  // y = xw+b
    MyMatrix u;  // u = active_func(y)；此層輸出(下一層的輸入)
    MyMatrix w, b;
    MyMatrix delta;

    MyMatrix grad_w;
    MyMatrix grad_b;

    ActiveFunc *active_func;
    Optimizer *optimizer;

    virtual MyMatrix forward(MyMatrix x) = 0;
    virtual MyMatrix backward(MyMatrix _delta) = 0;
    virtual void update() = 0;
};

class DenseLayer : public Layer{
public:
//    MyMatrix x;  // 輸入
//    MyMatrix y;  // y = xw+b
//    MyMatrix u;  // u = active_func(y)；此層輸出(下一層的輸入)
//    MyMatrix w, b;
//    MyMatrix delta;
//
//    MyMatrix grad_w;
//    MyMatrix grad_b;
//
//    ActiveFunc *active_func;
//    Optimizer *optimizer;

    DenseLayer(size_t input_size, size_t output_size, ActiveFunc *_activeFunc, Optimizer *_optimizer){
        init(input_size, output_size, _activeFunc, _optimizer);
    }

    void init(size_t input_size, size_t output_size, ActiveFunc *_activeFunc, Optimizer *_optimizer){
        w = MyMatrix(input_size, output_size, 0);
        w.random_matrix();
        b = MyMatrix(1, output_size, 0);
        b.random_matrix();
        grad_w = MyMatrix(w.row(), w.col(), 0);
        grad_b = MyMatrix(b.row(), b.col(), 0);
        active_func = _activeFunc;
        optimizer = _optimizer;
    }

    MyMatrix forward(MyMatrix _x) override{
        x = _x;
        u = MyMatrix::dot(&x, &w);
        u = MyMatrix::add(&u, &b);
        y = active_func->func_forward(u);
        return y;
    }

    MyMatrix backward(MyMatrix _delta) override{
//         x.T dot (_delta * active_func->func_backward(x))

        // 初始化gradient_w and b
        grad_w = MyMatrix(grad_w.row(), grad_w.col(), 0);
        grad_b = MyMatrix(grad_b.row(), grad_b.col(), 0);

        MyMatrix active_func_back = active_func->func_backward(u);
        MyMatrix my_delta = MyMatrix::times(&_delta, &active_func_back);

        MyMatrix x_t = x.transpose();
        MyMatrix _grad_w = MyMatrix::dot(&x_t, &my_delta);
        grad_w = MyMatrix::add(&grad_w, &_grad_w);

        MyMatrix _grad_b = my_delta;
        grad_b = MyMatrix::add(&grad_b, &_grad_b);
//1*30 3*30 =
        MyMatrix w_t = w.transpose();
        delta = MyMatrix::dot(&my_delta, &w_t);
//        grad_w.print_matrix();
        return delta;
    }

    void update() override{
        optimizer->gradient_decent(w, b, grad_w, grad_b);
    }

};

class MyFrame{
    Vector<Layer*> layers = Vector<Layer*>(0);
    LossFunc *lossFunc;
    int batch;

public:

    MyFrame(LossFunc *_lossFun, int _batch){
        lossFunc = _lossFun;
        batch = _batch;
    }

    ~MyFrame(){
        // 這裡是這樣寫嗎?
        layers.clear();
        layers = Vector<Layer*>();
    }

    void add(Layer *layer){
        layers.push_back(layer);
    }

    void train(size_t epoch, MyMatrix &x, MyMatrix &target){  // 這裡擴充batch size
        size_t _batch = batch == -1 ? x.row() : batch;

        for (size_t i = 0; i < epoch; i++){
            size_t data_left_size = x.row();  // 存著還有幾筆資料需要訓練
//            train_one_time(x, target);
//            continue;

            for (size_t j = 0; data_left_size > 0 ; j++){
                if (data_left_size < _batch){
                    // 如果資料量"不足"填滿一個batch
                    MyMatrix _x = MyMatrix::getRow(x, j * _batch, j * _batch + data_left_size);
                    MyMatrix _target = MyMatrix::getRow(target, j * _batch, j * _batch + data_left_size);

                    train_one_time(_x, _target);
                    data_left_size = 0;
                }else{
                    // 如果資料量"足夠"填滿一個batch
                    MyMatrix _x = MyMatrix::getRow(x, j * _batch, j * _batch + _batch);
                    MyMatrix _target = MyMatrix::getRow(target, j * _batch, j * _batch + _batch);

                    train_one_time(_x, _target);
                    data_left_size -= _batch;
                }

            }
        }
    }

    inline void train_one_time(MyMatrix &x, MyMatrix &target){  // 這裡客製化網路輸入
        MyMatrix y = x;
        for (size_t i=0;i<layers.size();++i){
            y = layers[i]->forward(y);
        }
        MyMatrix delta = lossFunc->backward(y, target);
        for (size_t i = layers.size() - 1; i >= 0; --i){
            delta = layers[i]->backward(delta);
        }

        for (size_t i = 0; i < layers.size(); ++i){
            layers[i]->update();
        }
    }

    void show(MyMatrix &x, MyMatrix &target){
        MyMatrix y = x;
        for (size_t i=0;i<layers.size();++i){
            y = layers[i]->forward(y);
        }

        // cout << "\nlabel: " << endl;
        target.print_matrix();

        // cout << "\nresult: " << endl;
        y.print_matrix();

        // cout << "\nloss: " << lossFunc->forward(y, target) << endl;

    }

};

#endif