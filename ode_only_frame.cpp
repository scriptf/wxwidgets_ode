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
#include <wx/kbdstate.h>

#include <deque>
////////////////////////
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <bits/stdc++.h>

class MyFrame : public wxFrame
{
public:
    //MyFrame(const wxString &title, const wxPoint &pos, const wxSize &size);
    //MyFrame(const wxString &title, const wxPoint &pos, const wxSize &size) : wxFrame((wxFrame *)NULL, -1, title, pos, size);

    //void OnPaint(wxPaintEvent &event);
    //void OnKeyDown(wxKeyEvent &event);

    std::deque<double> pointsX;
    std::deque<double> pointsY;
    std::deque<double> pointsNewX;
    std::deque<double> pointsNewY;
    int addX = 0;
    int addY = 0;
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

        if (counter > divider)
        {
            counter = 0;
            t = newT = 0.0;
        }

        return 0;
    }

    MyFrame(const wxString &title, const wxPoint &pos, const wxSize &size)
        : wxFrame((wxFrame *)NULL, -1, title, pos, size)
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
        //this->Connect(wxEVT_PAINT, wxPaintEventHandler(MyFrame::OnPaint));
        this->Bind(wxEVT_PAINT, [this](wxPaintEvent &) { OnPaint(); });

        this->SetBackgroundColour(wxColour(*wxWHITE));
        this->Bind(wxEVT_CHAR_HOOK, &MyFrame::OnKeyDown, this);
    }
    void OnPaint(wxPaintEvent &event)
    {
        //wxPaintDC dc(this);
        // http://vivacocoa.jp/cpp/wxwidgets/wxwidgets_graphics.php
        wxPaintDC *dc = new wxPaintDC(this);
        dc->SetPen(wxPen(wxColour("red"), 1, wxSOLID)); // Color of pen

        wxCoord x1 = 50, y1 = 60;
        wxCoord x2 = 190, y2 = 60;
        dc->DrawLine(x1, y1, x2, y2);

        wxSize size = wxWindow::GetSize();
        /*
        std::cerr << "width=" << size.x << std::endl;
        std::cerr << "height=" << size.y << std::endl;
        std::cerr << "pointsX=" << pointsX.size() << std::endl;
        */

        while (true)
        {
            for (int i = 0; i < int(pointsX.size()) - 1; i++)
            {
                //dc.DrawLine(pointsX[i],pointsY[i], pointsNewX[i], pointsNewY[i]);
                dc->DrawLine(
                    (pointsX[i]) * scale * zoom + size.x / 2 + addX,
                    -(pointsY[i]) * scale * zoom + size.y / 2 + addY,
                    (pointsNewX[i]) * scale * zoom + size.x / 2 + addX,
                    -(pointsNewY[i]) * scale * zoom + size.y / 2 + addY);
            }

            runge();
            //std::cerr << "value=" << value << std::endl;
            pointsX.push_back(x);
            pointsY.push_back(y);
            pointsNewX.push_back(newX);
            pointsNewY.push_back(newY);

            // 描画する点の数を制限する
            if (pointsX.size() > 500)
            {
                pointsX.pop_front();
                pointsNewX.pop_front();
                pointsY.pop_front();
                pointsNewY.pop_front();
            }
        }
        //        Refresh();
    }

    void OnKeyDown(wxKeyEvent &event)
    {
        if (event.ShiftDown() && wxGetKeyState(wxKeyCode('z'))) // zoom out
        {
            this->zoom = this->zoom - 0.1;
        }
        else if (wxGetKeyState(wxKeyCode('z'))) // zoom in
        {
            this->zoom = this->zoom + 0.1;
        }
        else if (wxGetKeyState(wxKeyCode('1')))
        {
            this->zoom = this->zoom + 0.1;
        }
        else if (wxGetKeyState(wxKeyCode('2')))
        {
            //wxMessageBox(wxString::Format("KeyDown: %i\n", (int)event.GetKeyCode()));
            this->zoom = this->zoom - 0.1;
        }
        else if (wxGetKeyState(WXK_RIGHT))
        {
            this->addX = this->addX + 1;
        }
        else if (wxGetKeyState(WXK_LEFT))
        {
            this->addX = this->addX - 1;
        }
        else if (wxGetKeyState(WXK_UP))
        {
            this->addY = this->addY - 1;
        }
        else if (wxGetKeyState(WXK_DOWN))
        {
            this->addY = this->addY + 1;
        }
        else if (wxGetKeyState(wxKeyCode('d'))) // default position
        {
            this->addX = 0;
            this->addY = 0;
            this->zoom = 1.0;
        }

        event.Skip();
    }
};

class MyApp : public wxApp
{
public:
    virtual bool OnInit()
    {
        //        MyFrame *frame = new MyFrame("Hello World", wxPoint(50, 50), wxSize(450, 340));
        //MyFrame *frame = new MyFrame("ODE", wxDefaultPosition, wxSize(640, 480));
        MyFrame *frame = new MyFrame("ODE", wxDefaultPosition, wxSize(1024, 640));

        frame->Show(TRUE);
        SetTopWindow(frame);
        return TRUE;
    }
};

IMPLEMENT_APP(MyApp)

//wxIMPLEMENT_APP(MyApp);
