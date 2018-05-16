#ifndef WINDOWCLASS_H_INCLUDED
#define WINDOWCLASS_H_INCLUDED

/*
    Copyright © 2000 Andrei Pangratie <andrpangr@yahoo.com>
This work is free. You can redistribute it and/or modify it under the
terms of the Do What The Fuck You Want To Public License, Version 2,
as published by Sam Hocevar. See the COPYING file for more details.
*/

#include <iostream>                 // needs iostream for some debuging
#include <windows.h>                // needs windows.h
#include <windowsx.h>               // needs windowsx.h

#include <gl/gl.h>                  // needs opengl for opengl Window
#include <gl/glu.h>

#include <map>

using namespace std;

// You should have opengl and Windows for this to work. If you dont want to use opengl coment the OpenGLWindow class.
// if using codeblocks like I do just link libgdi32.a , libopengl32.a , libglu32.a and it should work

class mouse{

        public :

    bool LMB ;
    bool RMB ;
    bool MMB ;

    float MMB_x0 ;
    float MMB_x ;

    float mouse_x ;
    float mouse_y ;

    float mouse_x0 ;
    float mouse_y0 ;

    mouse set_XY( float xp , float yp ){

        mouse_x0 = mouse_x;
        mouse_y0 = mouse_y;

        mouse_x = xp;
        mouse_y = yp;
    }

    mouse set_MMBmouve( float mouve ){

        MMB_x0 = MMB_x;
        MMB_x = mouve;
    }

    Point2f getPos(){
        return Point2f( mouse_x, mouse_y );
    }

    friend std::ostream& operator << ( std::ostream& stream , mouse& Mouse ){

        stream << "LMB " << Mouse.LMB << std::endl;
        stream << "RMB " << Mouse.RMB << std::endl;
        stream << "MMB " << Mouse.MMB << std::endl;

        stream << "x0 " << Mouse.mouse_x0 << " y0 " << Mouse.mouse_y0 << std::endl ;
        stream << "x  " << Mouse.mouse_x << " y  " << Mouse.mouse_y << std::endl ;

        stream << "MMB_last " << Mouse.MMB_x0 << " MMB_now " << Mouse.MMB_x << std::endl ;

        return stream ;

    }

    mouse(){
        LMB= RMB = MMB = 0;
    }

    ~mouse(){

    }

};

class keyboard{

        public :

    bool Key [ 256 ] ;

    keyboard (){
        for( int i = 0 ; i < 256 ; i++ ){
            Key[i] = false ;
        }
    }

    ~keyboard(){

    }

    bool &operator []( int i ){
        return Key[i];
    }
};

class EventMemory{
public:
    mouse Mouse;
    keyboard KeyBoard;

    bool WindowResized;

    EventMemory(){}

    virtual LRESULT ManageMsg( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam ){
        switch ( msg ){
            case WM_CLOSE :         PostQuitMessage(0);
            break ;

            case WM_DESTROY :       return 0 ;
            break ;

            case WM_KEYDOWN :       KeyBoard[ wParam ] = true ;
            break ;

            case WM_KEYUP :         KeyBoard[ wParam ] = false ;
            break ;

            case WM_MOUSEMOVE :     Mouse.set_XY( (float)GET_X_LPARAM( lParam ) , (float)GET_Y_LPARAM( lParam ) );
            break ;

            case WM_MOUSEWHEEL :    Mouse.set_MMBmouve( GET_WHEEL_DELTA_WPARAM( wParam ) );
            break ;

            case WM_LBUTTONDOWN :   Mouse.LMB = true ;
            break ;

            case WM_MBUTTONDOWN :   Mouse.MMB = true ;
            break ;

            case WM_RBUTTONDOWN :   Mouse.RMB = true ;
            break ;

            case WM_LBUTTONUP :     Mouse.LMB = false ;
            break ;

            case WM_MBUTTONUP :     Mouse.MMB = false ;
            break ;

            case WM_RBUTTONUP :     Mouse.RMB = false ;
            break ;

            case WM_SIZE :          WindowResized = true ;
            break ;

            default :               return DefWindowProc(hwnd, msg, wParam, lParam);
        }

        return 0 ;
    }
};

namespace EventManager{
    map < HWND, EventMemory* > ProcHandlers;

    void addEvent( HWND hwnd, EventMemory *eventMemory = new EventMemory() ){
        ProcHandlers.insert( pair< HWND, EventMemory* >( hwnd, eventMemory ) );
    }

    LRESULT CALLBACK EventProc( HWND hwnd , UINT msg, WPARAM wParam, LPARAM lParam){
        if( ProcHandlers.find( hwnd ) == ProcHandlers.end() )
            return DefWindowProc(hwnd, msg, wParam, lParam);

        return (ProcHandlers[hwnd])->ManageMsg( hwnd, msg, wParam, lParam );
    }
};

class Window {

        public :

    WNDCLASSEX wcex ;

    HWND hwnd ;
    HDC  hDC ;
    HGLRC hRC ;

    MSG msg ;
    RECT rect;

	DWORD dwExStyle ;
	DWORD dwStyle ;

	HINSTANCE hInstance;
	HINSTANCE hPrevInstance;
    LPSTR lpCmdLine ;
	int nCmdShow ;

	int Width ;
	int Height ;

	char *WindowName ;
	char *MenuName ;

	EventMemory *eventMemory;

    Window(){

        dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE ;
		dwStyle = WS_OVERLAPPEDWINDOW |	WS_CLIPSIBLINGS | WS_CLIPCHILDREN ;

		hInstance = GetModuleHandle(NULL);

	}

	Window( int width, int height, HWND hwndParent = NULL
            , EventMemory *eventMemory = new EventMemory(), char *wName = "WindowsOpengl" )
    {
        this->eventMemory = eventMemory;

        dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE ;
		dwStyle = WS_OVERLAPPEDWINDOW |	WS_CLIPSIBLINGS | WS_CLIPCHILDREN ;

		hInstance = GetModuleHandle(NULL);
		RegisterWCEX( CS_HREDRAW | CS_VREDRAW | CS_OWNDC, wName , "MenuGL" );
		CreateWindowHWND( wName , width , height, hwndParent );
        EventManager::addEvent( hwnd, eventMemory );
		WindowShow( SW_SHOW , false );
	}

	~Window() {
        DestroyWin();
	}

    void getWinApiProp( HINSTANCE hInstance , HINSTANCE hPrevInstance , LPSTR lpCmdLine , int nCmdShow ) {
        this->hInstance = hInstance  ;
        this->hPrevInstance = hPrevInstance ;
        this->lpCmdLine = lpCmdLine ;
        this->nCmdShow = nCmdShow ;
    }

    void RegisterWCEX( unsigned int style , char *ClassName , char* MenuName ) {
        this->WindowName = ClassName ;
        this->MenuName = MenuName ;

        this->wcex.cbSize = sizeof( WNDCLASSEX ) ;
        this->wcex.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC ;
        this->wcex.lpfnWndProc = EventManager::EventProc ;
        this->wcex.cbClsExtra = 0 ;
        this->wcex.cbWndExtra = 0 ;

        this->wcex.hInstance = this->hInstance ;

        this->wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION) ;
        this->wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION) ;
        this->wcex.hCursor = LoadCursor(NULL, IDC_ARROW) ;
        this->wcex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH) ;

        this->wcex.lpszMenuName = MenuName ;
        this->wcex.lpszClassName = WindowName ;

        RegisterClassEx( &wcex );
    }

    void CreateWindowHWND( char *TabName , int width = 640 , int height = 480, HWND hwndParent = NULL ){

        this->Width = width ;
        this->Height = height ;

        this->hwnd = CreateWindowEx(	dwExStyle ,
                                        WindowName ,
                                        TabName ,
                                        dwStyle,
                                        GetSystemMetrics(SM_CXSCREEN) / 2 - Width / 2 ,
                                        GetSystemMetrics(SM_CYSCREEN) / 2 - Height / 2 ,
                                        Width,
                                        Height,
                                        hwndParent,
                                        NULL,
                                        hInstance,
                                        NULL
                                    );

    }

    void WindowShow( int Flag , bool FirstTime = true ){

        if( FirstTime ){

            ShowWindow( this->hwnd , nCmdShow );

        }else{

            ShowWindow( this->hwnd , Flag );

        }
    }

    void DestroyWin(){

        wglMakeCurrent( NULL , NULL );
        wglDeleteContext( hRC );
        ReleaseDC( hwnd , hDC );

        DestroyWindow( hwnd );
    }

    bool WindowResize(){

        if( eventMemory->WindowResized == true ){

            if( GetClientRect( hwnd , &rect) )
            {
                Width = rect.right - rect.left ;
                Height = rect.bottom - rect.top ;
            }

            eventMemory->WindowResized = false ;

            return true ;

        }

        return false ;
    }

};

class OpenGLWindow : public Window {

        public :

    PIXELFORMATDESCRIPTOR pfd ;
    GLuint PixelFormat ;

    int ColorDepth ;
    int DepthBuffer ;
    int StencilBuffer ;

    int Flags ;

    OpenGLWindow ( bool perspective, int width , int height, HWND hwndParent = NULL,
                  EventMemory *eventMemory = new EventMemory(), char *wName = "WindowsOpengl" )
                  : Window( width, height, hwndParent, eventMemory, wName ){

        InitOpenGL( perspective, 32 , 32 );

    }

    ~OpenGLWindow() {

        DestroyWin();

    }

    void InitOpenGL( bool perspective, int ColorDepth = 32 , int DepthBuffer = 32 , int StencilBuffer = 32 , int Flags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER  ){


        this->DepthBuffer = DepthBuffer ;
        this->StencilBuffer = StencilBuffer ;
        this->ColorDepth = ColorDepth ;

        this->Flags = Flags ;

        pfd = {
                sizeof(PIXELFORMATDESCRIPTOR),
                1,
                Flags,
                PFD_TYPE_RGBA,
                ColorDepth,
                0, 0, 0, 0, 0, 0,
                0,
                0,
                0,
                0, 0, 0, 0,
                DepthBuffer,
                StencilBuffer,
                0,
                PFD_OVERLAY_PLANE,
                0,
                0, 0, 0
              };

        if (!(hDC=GetDC(hwnd)))
        {
            DestroyWin();
            MessageBox(NULL,"Can't Create A GL Device Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);

        }

        if (!(PixelFormat=ChoosePixelFormat(hDC,&pfd)))
        {
            DestroyWin();
            MessageBox(NULL,"Can't Find A Suitable PixelFormat.","ERROR",MB_OK|MB_ICONEXCLAMATION);

        }

        if(!SetPixelFormat(hDC,PixelFormat,&pfd))
        {
            DestroyWin();
            MessageBox(NULL,"Can't Set The PixelFormat.","ERROR",MB_OK|MB_ICONEXCLAMATION);

        }

        if (!(hRC=wglCreateContext(hDC)))
        {
            DestroyWin();
            MessageBox(NULL,"Can't Create A GL Rendering Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);

        }

        if(!wglMakeCurrent(hDC,hRC))
        {
            DestroyWin();
            MessageBox(NULL,"Can't Activate The GL Rendering Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);

        }

        ReSizeGLScene( perspective );

    }

    bool ReSizeGLScene( bool perspective = 0 , float angle = 55.0f , float start = .10f , float end = 50000.0f )
    {
        bool WasResized = false;

        if( WindowResize() ){

            WasResized = true;

            if (Height==0)
            {
                Height=1;
            }

            glViewport(0,0,Width,Height);

            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();

            if( perspective )
            {
                glLoadIdentity();
                gluPerspective( angle ,(float)Width/(float)Height , start , end );
            }

            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();
        }

        return WasResized;
    }

    void setVSync(bool sync)
    {

        typedef BOOL (APIENTRY *PFNWGLSWAPINTERVALPROC)( int );
        PFNWGLSWAPINTERVALPROC wglSwapIntervalEXT = 0;

        const char *extensions = (char*)glGetString( GL_EXTENSIONS );

        if( strstr( extensions, "WGL_EXT_swap_control" ) == 0 )
        {
            return;
        }
        else
        {
            wglSwapIntervalEXT = (PFNWGLSWAPINTERVALPROC)wglGetProcAddress( "wglSwapIntervalEXT" );

            if( wglSwapIntervalEXT )
                wglSwapIntervalEXT(sync);
        }

    }

};

#endif
