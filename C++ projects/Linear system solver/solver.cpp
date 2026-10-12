#include <bits/stdc++.h>

using namespace std;

class Rational{

    public:
    long long numerator;
    long long denominator;

    Rational simplify(){
        long long g = gcd(numerator, denominator);
        return Rational(numerator / g, denominator / g);
    }

    Rational invert(){
        return Rational(denominator, numerator);
    }

    Rational(long long num){
        this->numerator = num;
        this->denominator = 1;
    }

    Rational(long long numerator, long long denominator){
        if(denominator > 0){
            this->numerator = numerator;
            this->denominator = denominator;
        } else {
            this->numerator = -numerator;
            this->denominator = -denominator;
        }
        long long g = gcd(numerator, denominator);
        this->numerator /= g;
        this->denominator /= g;
    }

    Rational operator+(Rational other){
        long long commondenom = lcm(denominator, other.denominator);
        long long num1 = numerator * (commondenom / denominator);
        long long num2 = other.numerator * (commondenom / other.denominator);
        return Rational(num1 + num2, commondenom).simplify();
    }

    Rational operator-(){
        return Rational(-numerator, denominator);
    }

    Rational operator-(Rational other){
        return (*this) + (-other);
    }

    Rational operator*(Rational other){
        return Rational(numerator * other.numerator, denominator * other.denominator).simplify();
    }

    Rational operator/(Rational other){
        return (*this) * other.invert();
    }

    string toString(){
        return to_string(numerator) + "/" + to_string(denominator);
    }

    bool operator==(Rational other){
        Rational red1 = this->simplify();
        Rational red2 = other.simplify();
        return red1.numerator == red2.numerator && red1.denominator == red2.denominator;
    }

    bool operator!=(Rational other){
        Rational red1 = this->simplify();
        Rational red2 = other.simplify();
        return red1.numerator != red2.numerator || red1.denominator != red2.denominator;
    }

    bool operator>(Rational other){
        long long commondenom = lcm(denominator, other.denominator);
        long long num1 = numerator * (commondenom / denominator);
        long long num2 = other.numerator * (commondenom / other.denominator);
        return num1 > num2;
    }

    bool operator<(Rational other){
        long long commondenom = lcm(denominator, other.denominator);
        long long num1 = numerator * (commondenom / denominator);
        long long num2 = other.numerator * (commondenom / other.denominator);
        return num1 < num2;
    }

    bool operator>=(Rational other){
        long long commondenom = lcm(denominator, other.denominator);
        long long num1 = numerator * (commondenom / denominator);
        long long num2 = other.numerator * (commondenom / other.denominator);
        return num1 >= num2;
    }

    bool operator<=(Rational other){
        long long commondenom = lcm(denominator, other.denominator);
        long long num1 = numerator * (commondenom / denominator);
        long long num2 = other.numerator * (commondenom / other.denominator);
        return num1 <= num2;
    }

};

class System{
    
    public:

    long long N;
    vector<vector<Rational>> matrix;
    vector<Rational> equals;    

    System(long long N){
        matrix = vector<vector<Rational>>(N, vector<Rational>(N, Rational(0)));
        equals = vector<Rational>(N, Rational(0));
        this->N = N;
    }

    long long addZero(long long row){ //0 if successful, 1 if no solution (all zero row and nonzero equals), 2 if infinitely many solutions (all zero row and zero equals)
        long long numofzeros = 0;
        for(long long i = 0; i < N; i++){
            if(matrix[row][i] == Rational(0)){
                numofzeros++;
            } else {
                break;
            }
        }
        if(numofzeros == N){
            if(equals[row] == Rational(0)) return 2;
            else return 1;
        };
        if(numofzeros >= row) return 0;
        Rational coef = matrix[numofzeros][numofzeros] / matrix[row][numofzeros];
        for(long long i = numofzeros; i < N; i++){
            matrix[row][i] = matrix[row][i] * coef;
        }
        equals[row] = equals[row] * coef;

        for(long long i = numofzeros; i < N; i++){
            matrix[row][i] = matrix[row][i] - matrix[numofzeros][i];
        }
        equals[row] = equals[row] - equals[numofzeros];

        return 0;

    }

    long long rowEchelon(){
        bool flag = false;
        for(long long i = 1; i < N && !flag; i++){
            for(long long j = i; j < N && !flag; j++){
                long long returncode = addZero(j);
                if(returncode != 0){
                    flag = true;
                    if(returncode == 1) std::cout << "NO SOLUTION TO SYSTEM\n";
                    else std::cout << "INFINITELY MANY SOLUTIONS TO SYSTEM\n";
                }
            }
        }
        if(flag) return -1;
        long long numofzeros = 0;
        for(long long i = 0; i < N; i++){
            if(matrix[N-1][i] == Rational(0)){
                numofzeros++;
            } else {
                break;
            }
        }
        if(numofzeros == N){
            if(equals[N-1] == Rational(0)) std::cout << "INFINITELY MANY SOLUTIONS TO SYSTEM\n";
            else std::cout << "NO SOLUTION TO SYSTEM\n";
            return -1;
        }
        return 0;
    }

    void divBothSides(long long row){ //ONLY USE IF ALL VALUES OTHER THAN TARGET VARIABLE COEF ARE ZERO
        equals[row] = equals[row] / matrix[row][row];
        matrix[row][row] = Rational(1);        
    }

    void substitute(long long row){ //ONLY USE IF IN ROW-ECHELON FORM AND ALL BELOW ROWS WERE SOLVED
        for(long long i = row + 1; i < N; i++){
            equals[row] = equals[row] - (matrix[row][i] * equals[i]);
            matrix[row][i] = Rational(0);
        }
    }

    void solveRow(long long row){
        substitute(row);
        divBothSides(row);
    }

    void solve(){
        long long returncode = rowEchelon();
        if(returncode == -1) return;
        for(long long i = N-1; i >= 0; i--){
            solveRow(i);
        }
    }

    void print(){
        for(long long i = 0; i < N; i++){
            for(long long j = 0; j < N; j++){
                std::cout << matrix[i][j].toString() << "  ";
            }
            std::cout << ":  " << equals[i].toString() << "\n";
        }
    }

};

int main(){

    System system(10);

    system.matrix = {
        {Rational(2),  Rational(1),  Rational(-3), Rational(2),  Rational(1),
         Rational(-1), Rational(2), Rational(1), Rational(-1), Rational(3)},

        {Rational(1),  Rational(-2), Rational(1), Rational(3), Rational(-2),
         Rational(1), Rational(1), Rational(-2), Rational(3), Rational(-1)},

        {Rational(3),  Rational(1), Rational(2), Rational(-1), Rational(1),
         Rational(2), Rational(-1), Rational(1), Rational(2), Rational(1)},

        {Rational(-1), Rational(3), Rational(1), Rational(2), Rational(1),
         Rational(-1), Rational(3), Rational(-2), Rational(1), Rational(2)},

        {Rational(2),  Rational(-1), Rational(1), Rational(1), Rational(3),
         Rational(2), Rational(1), Rational(-1), Rational(-2), Rational(1)},

        {Rational(1),  Rational(2), Rational(-1), Rational(-1), Rational(2),
         Rational(3), Rational(-1), Rational(1), Rational(1), Rational(-2)},

        {Rational(-2), Rational(1), Rational(3), Rational(1), Rational(-1),
         Rational(1), Rational(2), Rational(3), Rational(-1), Rational(1)},

        {Rational(1),  Rational(-1), Rational(2), Rational(3), Rational(-1),
         Rational(2), Rational(1), Rational(1), Rational(3), Rational(-2)},

        {Rational(3),  Rational(2), Rational(-1), Rational(1), Rational(2),
         Rational(-1), Rational(1), Rational(-3), Rational(2), Rational(1)},

        {Rational(-1), Rational(2), Rational(1), Rational(-1), Rational(3),
         Rational(1), Rational(-2), Rational(1), Rational(1), Rational(2)}
    };

    system.equals = {
        Rational(22),
        Rational(-6),
        Rational(10),
        Rational(-8),
        Rational(23),
        Rational(14),
        Rational(-5),
        Rational(9),
        Rational(16),
        Rational(7)
    };

    system.print();

    auto start = chrono::steady_clock::now();
    system.solve();
    auto end = chrono::steady_clock::now();

    chrono::duration<double, milli> elapsed = end - start;

    std::cout << "\n";

    system.print();
    std::cout << "Solved system in " << elapsed.count() << " ms\n";
}