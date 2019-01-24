#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#if !wxUSE_WEBVIEW_WEBKIT && !wxUSE_WEBVIEW_WEBKIT2 && !wxUSE_WEBVIEW_IE
#error "A wxWebView backend is required!"
#endif

#include "wx/cmdline.h"
#include "wx/webview.h"

class WebApp : public wxApp
{
public:
    WebApp() :
        m_url("http://www.wxwidgets.org")
    {
    }

    virtual bool OnInit();

    virtual void OnInitCmdLine(wxCmdLineParser& parser)
    {
        wxApp::OnInitCmdLine(parser);

        parser.AddOption("W",
                         "width",
                         _("WIDTH of Window"),
                         wxCMD_LINE_VAL_NUMBER,
                         wxCMD_LINE_PARAM_OPTIONAL);

        parser.AddOption("H",
                         "height",
                         _("HEIGHT of Window"),
                         wxCMD_LINE_VAL_NUMBER,
                         wxCMD_LINE_PARAM_OPTIONAL);

        parser.AddParam("URL",
                        wxCMD_LINE_VAL_STRING,
                        wxCMD_LINE_PARAM_OPTIONAL);
    }

    virtual bool OnCmdLineParsed(wxCmdLineParser& parser)
    {
        if ( !wxApp::OnCmdLineParsed(parser) )
            return false;

        if ( parser.GetParamCount() )
            m_url = parser.GetParam(0);

        parser.Found("W", &m_width);
        parser.Found("H", &m_height);

        return true;
    }

private:
    wxString m_url;
    long m_width;
    long m_height;
};

class WebFrame : public wxFrame
{
public:
    WebFrame(const wxString& url, long width, long height);
    virtual ~WebFrame();

    void UpdateState();
    void OnIdle(wxIdleEvent& evt);
    void OnNavigationRequest(wxWebViewEvent& evt);
    void OnNavigationComplete(wxWebViewEvent& evt);
    void OnDocumentLoaded(wxWebViewEvent& evt);
    void OnNewWindow(wxWebViewEvent& evt);
    void OnTitleChanged(wxWebViewEvent& evt);
    void OnSetZoom(wxCommandEvent& evt);
    void OnError(wxWebViewEvent& evt);
    void OnZoomLayout(wxCommandEvent& evt);

private:
    wxWebView* m_browser;

    wxMenu* m_tools_menu;
    wxMenuItem* m_tools_layout;
    wxMenuItem* m_tools_tiny;
    wxMenuItem* m_tools_small;
    wxMenuItem* m_tools_medium;
    wxMenuItem* m_tools_large;
    wxMenuItem* m_tools_largest;
    wxMenuItem* m_context_menu;
};

IMPLEMENT_APP(WebApp)

// ============================================================================
// implementation
// ============================================================================

bool WebApp::OnInit()
{
    wxString title(_("Plop"));

    this->SetAppName(title);

    if ( !wxApp::OnInit() )
        return false;

    // Create the memory files
    wxImage::AddHandler(new wxPNGHandler);
    WebFrame *frame = new WebFrame(m_url, m_width, m_height);
    // like a dialog
    frame->SetWindowStyle(wxCENTRE | wxFRAME_TOOL_WINDOW | wxRESIZE_BORDER | wxSYSTEM_MENU | wxCLOSE_BOX | wxCLIP_CHILDREN | wxSTAY_ON_TOP);

    wxMenu* mainMenu = new  wxMenu();
    mainMenu->Append(wxID_EXIT, _("Quit\tCtrl-Q"));

#if defined(__WXOSX__)
    wxMenuBar* menubar = new wxMenuBar();
    menubar->Append(mainMenu, _("View"));
    wxMenuBar::MacSetCommonMenuBar(menubar);
#endif

    frame->Center();
    frame->Show();
    return true;
}

WebFrame::WebFrame(const wxString& url, long width, long height) :
    wxFrame(NULL, wxID_ANY, _("wxWebView Sample"))
{
    // set the frame icon
    // SetTitle("Drag URL to Open");

    wxBoxSizer* topsizer = new wxBoxSizer(wxVERTICAL);

    // Create the webview
    m_browser = wxWebView::New(this, wxID_ANY, url);
    topsizer->Add(m_browser, wxSizerFlags().Expand().Proportion(1));

    SetSizer(topsizer);

    // 16:9 Movies
    SetSize(wxSize(width > 0 ? width : 512, height > 0 ? height : 288));

    // Create a log window
    // new wxLogWindow(this, _("Logging"), true, false);

    // Create the Tools menu
    m_tools_menu = new wxMenu();
    m_tools_layout = m_tools_menu->AppendCheckItem(wxID_ANY, _("Use Layout Zoom"));
    m_tools_tiny = m_tools_menu->AppendCheckItem(wxID_ANY, _("Tiny"));
    m_tools_small = m_tools_menu->AppendCheckItem(wxID_ANY, _("Small"));
    m_tools_medium = m_tools_menu->AppendCheckItem(wxID_ANY, _("Medium"));
    m_tools_large = m_tools_menu->AppendCheckItem(wxID_ANY, _("Large"));
    m_tools_largest = m_tools_menu->AppendCheckItem(wxID_ANY, _("Largest"));
    m_tools_menu->AppendSeparator();

    if(!m_browser->CanSetZoomType(wxWEBVIEW_ZOOM_TYPE_LAYOUT))
        m_tools_layout->Enable(false);

    wxMenuBar* menubar = new wxMenuBar();
    menubar->Append(m_tools_menu, _("View"));
    this->SetMenuBar(menubar);

    // Connect the webview events
    Connect(m_browser->GetId(), wxEVT_WEBVIEW_NAVIGATING, wxWebViewEventHandler(WebFrame::OnNavigationRequest), NULL, this);
    Connect(m_browser->GetId(), wxEVT_WEBVIEW_NAVIGATED, wxWebViewEventHandler(WebFrame::OnNavigationComplete), NULL, this);
    Connect(m_browser->GetId(), wxEVT_WEBVIEW_LOADED, wxWebViewEventHandler(WebFrame::OnDocumentLoaded), NULL, this);
    Connect(m_browser->GetId(), wxEVT_WEBVIEW_ERROR, wxWebViewEventHandler(WebFrame::OnError), NULL, this);
    Connect(m_browser->GetId(), wxEVT_WEBVIEW_NEWWINDOW, wxWebViewEventHandler(WebFrame::OnNewWindow), NULL, this);
    Connect(m_browser->GetId(), wxEVT_WEBVIEW_TITLE_CHANGED, wxWebViewEventHandler(WebFrame::OnTitleChanged), NULL, this);

    // Connect the menu events
    Connect(m_tools_layout->GetId(), wxEVT_MENU, wxCommandEventHandler(WebFrame::OnZoomLayout),  NULL, this );
    Connect(m_tools_tiny->GetId(), wxEVT_MENU, wxCommandEventHandler(WebFrame::OnSetZoom),  NULL, this );
    Connect(m_tools_small->GetId(), wxEVT_MENU, wxCommandEventHandler(WebFrame::OnSetZoom),  NULL, this );
    Connect(m_tools_medium->GetId(), wxEVT_MENU, wxCommandEventHandler(WebFrame::OnSetZoom),  NULL, this );
    Connect(m_tools_large->GetId(), wxEVT_MENU, wxCommandEventHandler(WebFrame::OnSetZoom),  NULL, this );
    Connect(m_tools_largest->GetId(), wxEVT_MENU, wxCommandEventHandler(WebFrame::OnSetZoom),  NULL, this );

    //Connect the idle events
    Connect(wxID_ANY, wxEVT_IDLE, wxIdleEventHandler(WebFrame::OnIdle), NULL, this);
}

WebFrame::~WebFrame()
{
    // delete m_tools_menu;
}

/**
  * Method that retrieves the current state from the web control and updates the GUI
  * the reflect this current state.
  */
void WebFrame::UpdateState()
{
    SetTitle( m_browser->GetCurrentTitle() );
}

void WebFrame::OnIdle(wxIdleEvent& WXUNUSED(evt))
{
    if(m_browser->IsBusy())
    {
        wxSetCursor(wxCURSOR_ARROWWAIT);
    }
    else
    {
        wxSetCursor(wxNullCursor);
    }
}

/**
  * Callback invoked when there is a request to load a new page (for instance
  * when the user clicks a link)
  */
void WebFrame::OnNavigationRequest(wxWebViewEvent& evt)
{
    // wxLogMessage("%s", "Navigation request to '" + evt.GetURL() + "' (target='" + evt.GetTarget() + "')");

    wxASSERT(m_browser->IsBusy());

    UpdateState();
}

/**
  * Callback invoked when a navigation request was accepted
  */
void WebFrame::OnNavigationComplete(wxWebViewEvent& evt)
{
    // wxLogMessage("%s", "Navigation complete; url='" + evt.GetURL() + "'");
    UpdateState();
}

/**
  * Callback invoked when a page is finished loading
  */
void WebFrame::OnDocumentLoaded(wxWebViewEvent& evt)
{
    //Only notify if the document is the main frame, not a subframe
    if(evt.GetURL() == m_browser->GetCurrentURL())
    {
        // wxLogMessage("%s", "Document loaded; url='" + evt.GetURL() + "'");
    }
    UpdateState();
}

/**
  * On new window, we veto to stop extra windows appearing
  */
void WebFrame::OnNewWindow(wxWebViewEvent& evt)
{
    // wxLogMessage("%s", "New window; url='" + evt.GetURL() + "'");
    m_browser->LoadURL(evt.GetURL());

    UpdateState();
}

void WebFrame::OnTitleChanged(wxWebViewEvent& evt)
{
    SetTitle(evt.GetString());
    // wxLogMessage("%s", "Title changed; title='" + evt.GetString() + "'");
}

/**
  * Invoked when user selects the zoom size in the menu
  */
void WebFrame::OnSetZoom(wxCommandEvent& evt)
{
    m_tools_tiny->Check(false);
    m_tools_small->Check(false);
    m_tools_medium->Check(false);
    m_tools_large->Check(false);
    m_tools_largest->Check(false);

    if (evt.GetId() == m_tools_tiny->GetId())
    {
        m_tools_tiny->Check();
        m_browser->SetZoom(wxWEBVIEW_ZOOM_TINY);
    }
    else if (evt.GetId() == m_tools_small->GetId())
    {
        m_tools_small->Check();
        m_browser->SetZoom(wxWEBVIEW_ZOOM_SMALL);
    }
    else if (evt.GetId() == m_tools_medium->GetId())
    {
        m_tools_medium->Check();
        m_browser->SetZoom(wxWEBVIEW_ZOOM_MEDIUM);
    }
    else if (evt.GetId() == m_tools_large->GetId())
    {
        m_tools_large->Check();
        m_browser->SetZoom(wxWEBVIEW_ZOOM_LARGE);
    }
    else if (evt.GetId() == m_tools_largest->GetId())
    {
        m_tools_largest->Check();
        m_browser->SetZoom(wxWEBVIEW_ZOOM_LARGEST);
    }
    else
    {
        wxFAIL;
    }
}

void WebFrame::OnZoomLayout(wxCommandEvent& WXUNUSED(evt))
{
    if(m_tools_layout->IsChecked())
        m_browser->SetZoomType(wxWEBVIEW_ZOOM_TYPE_LAYOUT);
    else
        m_browser->SetZoomType(wxWEBVIEW_ZOOM_TYPE_TEXT);
}

/**
  * Callback invoked when a loading error occurs
  */
void WebFrame::OnError(wxWebViewEvent& evt)
{
#define WX_ERROR_CASE(type) \
    case type: \
        category = #type; \
        break;

    wxString category;
    switch (evt.GetInt())
    {
        WX_ERROR_CASE(wxWEBVIEW_NAV_ERR_CONNECTION);
        WX_ERROR_CASE(wxWEBVIEW_NAV_ERR_CERTIFICATE);
        WX_ERROR_CASE(wxWEBVIEW_NAV_ERR_AUTH);
        WX_ERROR_CASE(wxWEBVIEW_NAV_ERR_SECURITY);
        WX_ERROR_CASE(wxWEBVIEW_NAV_ERR_NOT_FOUND);
        WX_ERROR_CASE(wxWEBVIEW_NAV_ERR_REQUEST);
        WX_ERROR_CASE(wxWEBVIEW_NAV_ERR_USER_CANCELLED);
        WX_ERROR_CASE(wxWEBVIEW_NAV_ERR_OTHER);
    }

    wxLogMessage("%s", "Error; url='" + evt.GetURL() + "', error='" + category + " (" + evt.GetString() + ")'");

    UpdateState();
}
