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
    double scale = 10;
    double zoom = 1.0;
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
        kk = 0.1;
        B = 13.388;
        x = newX = 4;
        y = newY = 0;

        divider = 180; // 2PIを割る数
        dt = 2.0 * M_PI / double(divider);
        counter = 0;
        num_term = 1000;
        steps = divider * num_term;
        plot = divider * 50;

        Bind(wxEVT_PAINT, &MyPanel::myOnPaint, this);
        Bind(wxEVT_TIMER, &MyPanel::myOnTimer, this, timer.GetId());
        //timer.Start(10); // msec
        timer.Start(1); // msec
    }

    void myOnPaint(wxPaintEvent &evt)
    {
        //wxPaintDC dc(this);
        // http://vivacocoa.jp/cpp/wxwidgets/wxwidgets_graphics.php
        wxPaintDC *dc = new wxPaintDC(this);
        dc->SetPen(wxPen(wxColour("red"), 1, wxSOLID)); // Color of pen

        wxSize size = wxWindow::GetSize();

        std::cerr << "width=" << size.x << std::endl;
        std::cerr << "height=" << size.y << std::endl;

        //dc.DrawLine(x, points[x], x + 1, points[x + 1]);
        //dc.DrawLine(x, y, newX, newY);

        std::cerr << "pointsX=" << pointsX.size() << std::endl;

        //dc->DrawLine(pointsX[0],pointsY[0], pointsNewX[0], pointsNewY[0]);

        for (int i = 0; i < int(pointsX.size()) - 1; i++)
        {
            //dc.DrawLine(pointsX[i],pointsY[i], pointsNewX[i], pointsNewY[i]);
            dc->DrawLine((pointsX[i])*scale + size.x / 2, -(pointsY[i])*scale  + size.y / 2,
                         (pointsNewX[i])*scale + size.x / 2, -(pointsNewY[i])*scale  + size.y / 2);
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
        // 計算の処理はこちらに書いたほうが描画が早い
        runge();

        //int value = wxGetMouseState().GetY() - GetScreenPosition().y;
        int value = int(y * 100 + 150);

        //std::cerr << "value=" << value << std::endl;
        pointsX.push_back(x);
        pointsY.push_back(y);
        pointsNewX.push_back(newX);
        pointsNewY.push_back(newY);

        // 描画する点の数を制限する
        if (pointsX.size() > 640)
        {
            pointsX.pop_front();
            pointsNewX.pop_front();
            pointsY.pop_front();
            pointsNewY.pop_front();
        }
        // 再描画する
        Refresh();
    }

private:
    wxTimer timer;
    //std::deque<int> points;
    std::deque<double> pointsX;
    std::deque<double> pointsY;
    std::deque<double> pointsNewX;
    std::deque<double> pointsNewY;
};

class MyApp : public wxApp
{
public:
    virtual bool OnInit()
    {
        wxFrame *frame = new wxFrame(NULL, wxID_ANY, "Hello, world!",
                                     wxDefaultPosition, wxSize(640, 480));
        MyPanel *panel = new MyPanel(frame);
        panel->SetBackgroundColour(wxColour(*wxWHITE));
        frame->Show();
        return true;
    }
};

wxIMPLEMENT_APP(MyApp);
