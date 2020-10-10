#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <bits/stdc++.h>

const double PI = acos(-1);

//double mu = 0.2;
double B;
//double nu = 4.0;
double kk;

double
x_dot(double t, double x, double y)
{
    return y;
}

double
y_dot(double t, double x, double y)
{
    // return -x;
    // return mu * (1 - x * x) * y - x * x * x + B * cos(nu * t);
    return -kk * y - x * x * x + B * cos(t);
}

int main(int argc, char *argv[])
{
    double dt, t, x, y;
    double k1, k2, k3, k4;
    double h1, h2, h3, h4;
    double k, h;

    if (argc != 7)
    {
        /////////////1 2 3 4 5            6
        std::cerr << "kk B x y divider(int) nun_term(int)" << std::endl;
        std::cerr << "0.015 0.45 1.5 0.0 180 1000" << std::endl;
        exit(0);
    }

    // 初期値
    t = 0.0;
    kk = atof(argv[1]);
    B = atof(argv[2]);
    x = atof(argv[3]);
    y = atof(argv[4]);
    //int divider = 180; // 2PIを割る数
    int divider = atoi(argv[5]); // 2PIを割る数
    dt = 2.0 * M_PI / double(divider);

    int counter = 0; // divider と等しくなるまで計算の回数をカウントする。等しくなったら0クリア
    //int num_term = 1000; // 2PI*num_term の期間を計算する
    int num_term = atoi(argv[6]); // 2PI*num_term の期間を計算する

    int steps = divider * num_term; // t について、2PI の steps 倍の期間について計算する
    int plot = divider * 50;        // プロットする期間(2PI*plot to steps*2PI)

    printf("t,x,y,stroboX,stroboY\n");
    for (int i = 0; i <= steps; i++)
    {
        // 4次のルンゲクッタ法
        k1 = dt * x_dot(t, x, y);
        h1 = dt * y_dot(t, x, y);

        k2 = dt * x_dot(t + dt / 2.0, x + k1 / 2.0, y + h1 / 2.0);
        h2 = dt * y_dot(t + dt / 2.0, x + k1 / 2.0, y + h1 / 2.0);

        k3 = dt * x_dot(t + dt / 2.0, x + k2 / 2.0, y + h2 / 2.0);
        h3 = dt * y_dot(t + dt / 2.0, x + k2 / 2.0, y + h2 / 2.0);

        k4 = dt * x_dot(t + dt, x + k3, y + h3);
        h4 = dt * y_dot(t + dt, x + k3, y + h3);

        ///////////////////////////////
        k = (k1 + 2.0 * k2 + 2.0 * k3 + k4) / 6.0;
        h = (h1 + 2.0 * h2 + 2.0 * h3 + h4) / 6.0;

        // next value
        t = t + dt;
        x = x + k;
        y = y + h;
        counter++;

        // 定常状態のみプロットする
        if (steps - i < plot)
        {
            // 誤差が蓄積しないようにt=2PIごとにt=0で初期化する
            if (counter % divider == 0)
            {
                printf("%d,%f,%f,%f,%f,%f\n", i, i * dt, x, y, x, y);
                t = 0.0;
                counter = 0;
            }
            else
            {
                printf("%d,%f,%f,%f,,\n", i, i * dt, x, y);
            }
        }
    }
    return 0;
}
