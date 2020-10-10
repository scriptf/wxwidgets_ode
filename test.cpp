// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/statline.h"
#include "wx/wfstream.h"
#include "wx/filedlg.h"
#include "wx/stockitem.h"
#include "wx/dcbuffer.h"

#include <deque>
////////////////////////
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <bits/stdc++.h>

// 軌跡の長さ

class MyPanel : public wxPanel
{
    const double PI = acos(-1);

    //double mu = 0.2;
    double B;
    //double nu = 4.0;
    double kk;
    double dt, t, x, y, newT, newX, newY;
    double k1, k2, k3, k4;
    double h1, h2, h3, h4;
    double k, h;

    //int divider = 180; // 2PIを割る数
    int divider; // 2PIを割る数

    int counter; // divider と等しくなるまで計算の回数をカウントする。等しくなったら0クリア
    //int num_term = 1000; // 2PI*num_term の期間を計算する
    int num_term; // 2PI*num_term の期間を計算する

    int steps; // t について、2PI の steps 倍の期間について計算する
    int plot;  // プロットする期間(2PI*plot to steps*2PI)

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

    int runge(void)
    {
        t = newT;
        x = newX;
        y = newY;
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
        newT = t + dt;
        newX = x + k;
        newY = y + h;
        counter++;

        return 0;
    }

public:
    MyPanel(wxFrame *parent)
        : wxPanel(parent),
          timer(this)
    {
        // 初期値
        t = newT = 0.0;
        kk = 0.015;
        B = 0.45;
        x = newX = 1.5;
        y = newY =  0;
        
        divider = 180; // 2PIを割る数
        dt = 2.0 * M_PI / double(divider);
        counter = 0;
        num_term = 1000;
        steps = divider * num_term;
        plot = divider * 50;

        Bind(wxEVT_PAINT, &MyPanel::myOnPaint, this);
        Bind(wxEVT_TIMER, &MyPanel::myOnTimer, this, timer.GetId());
        timer.Start(10); // msec
    }

    void myOnPaint(wxPaintEvent &evt)
    {
        wxPaintDC dc(this);

        runge();
        //dc.DrawLine(x, points[x], x + 1, points[x + 1]);
        //dc.DrawLine(x, y, newX, newY);
        

        for (int x = 0; x < int(pointsX.size()) - 1; ++x)
        {
            dc.DrawLine(x, pointsX[x], x + 1, pointsY[x + 1]);
        }
/*
        for (int x = 0; x < int(points.size()) - 1; ++x)
        {
            dc.DrawLine(x, points[x], x + 1, points[x + 1]);
        }
*/

    }

    void myOnTimer(wxTimerEvent &event)
    {
        //int value = wxGetMouseState().GetY() - GetScreenPosition().y;
        int value = int(y*100+150);

        //std::cerr << "value=" << value << std::endl;
        pointsX.push_back(int(X*100+150));
        pointsY.push_back(int(y*100+150));
        
        points.push_back(value);
        if (points.size() > 640)
        {
            points.pop_front();
        }
        Refresh();
    }

private:
    wxTimer timer;
    std::deque<int> points;
    std::deque<int> pointsX;
    std::deque<int> pointsY;
};

class MyApp : public wxApp
{
public:
    virtual bool OnInit()
    {
        wxFrame *frame = new wxFrame(NULL, wxID_ANY, "Hello, world!",
                                     wxDefaultPosition, wxSize(640, 480));
        MyPanel *panel = new MyPanel(frame);
        frame->Show();
        return true;
    }
};

wxIMPLEMENT_APP(MyApp);
