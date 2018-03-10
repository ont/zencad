// Moved the #include <OpenGl_GraphicsDriver.hxx> 
// and added 9 #undef lines to get to build in Linux
#include <OpenGl_GraphicDriver.hxx>
#undef Bool
#undef CursorShape
#undef None
#undef KeyPress
#undef KeyRelease
#undef FocusIn
#undef FocusOut
#undef FontChange
#undef Expose

// base header
#include <DisplayWidget.h>

// occ header files.
#include <V3d_View.hxx>
#include <V3d_AmbientLight.hxx>
#include <V3d_DirectionalLight.hxx>

#include <Aspect_Handle.hxx>
#include <Aspect_DisplayConnection.hxx>

#ifdef WNT
  #include <WNT_Window.hxx>
#elif defined(__APPLE__) && !defined(MACOSX_USE_GLX)
  #include <Cocoa_Window.hxx>
#else
  #include <Xw_Window.hxx>
#endif
#include <AIS_Shape.hxx>

// qt header files
#include <QtGui/QMouseEvent>

// gxx header files
#include <gxx/print.h>


static Handle(Graphic3d_GraphicDriver)& GetGraphicDriver() {
    static Handle(Graphic3d_GraphicDriver) aGraphicDriver;
    return aGraphicDriver;
}

const Handle_AIS_InteractiveContext& DisplayWidget::getContext() const {
    return m_context;    
}

void DisplayWidget::showEvent(QShowEvent* e) {
    QWidget::showEvent(e);
    //init();
    //m_view->MustBeResized();
    //m_view->Redraw();
}

void DisplayWidget::paintEvent(QPaintEvent* e) {
    Q_UNUSED(e);

    if (m_context.IsNull()) {
        init();
        for (auto& wrap : display_on_init_list) {
            Handle(AIS_Shape) anAisBox1 = new AIS_Shape(wrap->native);
            Handle(AIS_Shape) anAisBox2 = new AIS_Shape(wrap->native);

            Quantity_Color shpcolor (0.6, 0.6, 0.8,  Quantity_TOC_RGB);  
            anAisBox1->SetColor(shpcolor);
            getContext()->Display(anAisBox1, false);
            
            anAisBox2->SetColor(Quantity_NOC_BLACK);
            anAisBox2->SetDisplayMode(AIS_WireFrame);  
            getContext()->Display(anAisBox2, false);
            m_view->FitAll (0.5, false);
        }
    }

    m_view->Redraw();
}

void DisplayWidget::resizeEvent(QResizeEvent* e) {
    Q_UNUSED(e);

    if( !m_view.IsNull() ){
        m_view->MustBeResized();
    }
}

void DisplayWidget::init() {
    // Create Aspect_DisplayConnection
    Handle(Aspect_DisplayConnection) aDisplayConnection = new Aspect_DisplayConnection();

    // Get graphic driver if it exists, otherwise initialise it
    if (GetGraphicDriver().IsNull()) {
        GetGraphicDriver() = new OpenGl_GraphicDriver(aDisplayConnection);
    }

    // Get window handle. This returns something suitable for all platforms.
    WId window_handle = (WId) winId();

    // Create appropriate window for platform
    #ifdef WNT
        Handle(WNT_Window) wind = new WNT_Window((Aspect_Handle) window_handle);
    #elif defined(__APPLE__) && !defined(MACOSX_USE_GLX)
        Handle(Cocoa_Window) wind = new Cocoa_Window((NSView *) window_handle);
    #else
        Handle(Xw_Window) wind = new Xw_Window(aDisplayConnection, (Window) window_handle);
    #endif

    // Create V3dViewer and V3d_View
    m_viewer = new V3d_Viewer(GetGraphicDriver(), (Standard_ExtString)"viewer");

    m_view = m_viewer->CreateView();

    m_view->SetWindow(wind);
    if (!wind->IsMapped()) wind->Map();

    // Create AISInteractiveContext
    m_context = new AIS_InteractiveContext(m_viewer);

    // Set up lights etc
    //m_viewer->SetDefaultLights();
    m_viewer->SetLightOn (new V3d_DirectionalLight (m_viewer, V3d_Zneg , Quantity_NOC_WHITE, true));
    //m_viewer->SetLightOn(new V3d_AmbientLight (m_viewer, Quantity_NOC_BLUE1));

    //m_view->SetBackgroundColor(Quantity_NOC_FOREST);
    m_view->MustBeResized();
    m_view->TriedronDisplay(Aspect_TOTP_LEFT_LOWER, Quantity_NOC_GOLD, 0.08, V3d_ZBUFFER);

    //m_context->SetDisplayMode(AIS_Shaded, false);
    m_context->SetDisplayMode(AIS_Shaded, false);
    //m_context->SetHilightColor(Quantity_NOC_AZURE);

    m_view->SetBgGradientColors(
        Quantity_Color(0.5,0.5,0.5,Quantity_TOC_RGB), 
        Quantity_Color(0.3,0.3,0.3,Quantity_TOC_RGB), 
        Aspect_GFM_VER, 
        Standard_False
    );
}

void DisplayWidget::wheelEvent( QWheelEvent * e ) {
    onMouseWheel(e->buttons(), e->delta(), e->pos());
}

void DisplayWidget::mousePressEvent( QMouseEvent* e )
{
    if (e->button() == Qt::LeftButton) {
        onLButtonDown((e->buttons() | e->modifiers()), e->pos());
    }
    else if (e->button() == Qt::MidButton)
    {
        onMButtonDown((e->buttons() | e->modifiers()), e->pos());
    }
    else if (e->button() == Qt::RightButton)
    {
        onRButtonDown((e->buttons() | e->modifiers()), e->pos());
    }
}

void DisplayWidget::mouseReleaseEvent( QMouseEvent* e ) {

}

void DisplayWidget::mouseMoveEvent( QMouseEvent * e ) {
    onMouseMove(e->buttons(), e->pos());
}

void DisplayWidget::onMouseWheel( const int theFlags, const int theDelta, const QPoint thePoint )
{
    Q_UNUSED(theFlags);

    Standard_Integer aFactor = 16;

    Standard_Integer aX = thePoint.x();
    Standard_Integer aY = thePoint.y();

    if (theDelta > 0)
    {
        aX += aFactor;
        aY += aFactor;
    }
    else
    {
        aX -= aFactor;
        aY -= aFactor;
    }

    m_view->Zoom(thePoint.x(), thePoint.y(), aX, aY);
}

void DisplayWidget::onLButtonDown( const int theFlags, const QPoint thePoint ) {
    Q_UNUSED(theFlags);
    m_view->StartRotation(thePoint.x(), thePoint.y());
}

void DisplayWidget::onMButtonDown( const int theFlags, const QPoint thePoint ) {
    Q_UNUSED(theFlags);
    m_view->StartRotation(thePoint.x(), thePoint.y());
}

void DisplayWidget::onRButtonDown( const int theFlags, const QPoint thePoint ) {
    Q_UNUSED(theFlags);
    temporary1 = thePoint;
}

void DisplayWidget::onLButtonUp( const int theFlags, const QPoint thePoint ) {
}

void DisplayWidget::onMButtonUp( const int theFlags, const QPoint thePoint ) {
}

void DisplayWidget::onRButtonUp( const int theFlags, const QPoint thePoint ) {
}

void DisplayWidget::onMouseMove( const int theFlags, const QPoint thePoint ) {
    if (theFlags & Qt::LeftButton) {
        m_view->Rotation(thePoint.x(), thePoint.y());
    }

    if (theFlags & Qt::RightButton) {
        QPoint mv = thePoint - temporary1; 
        temporary1 = thePoint;
        m_view->Pan(mv.x(), -mv.y());
    }
}