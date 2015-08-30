#include "video.h"
#define RELEASEEX(x) if(x){x->Release();x=NULL;}

IGraphBuilder*            g_graph;        //FILTER管理器
IBaseFilter*              g_filter;         //用于创建VMR9
IBaseFilter*              g_source;        //用于流媒体数据处理
IEnumPins*                g_enumpins;     //用于获取PIN
IFilterGraph2*            g_Graph2;             //用于视频流渲染操作
IMediaControl*            g_mediaControl;   //媒体控制
IMediaEvent*              pEvent;
IVMRFilterConfig9*        g_filterConfig;     //VMR9配置
IVMRWindowlessControl9*   g_VMR;         //VMR9的WINDOWED模式控制
extern HWND               hwnd;                     //当前窗口句柄
IBaseFilter*        g_sound;


/***********************************/
/*Initialization of the variables via calls to their respective interfaces*/
bool initVideo(void)
{
	//创建FilterGraphManager
	if(FAILED(CoCreateInstance(CLSID_FilterGraph,NULL,CLSCTX_INPROC_SERVER,IID_IGraphBuilder,(void **)&g_graph)))
		return false;

	//创建FilterGraph(VMR9)
	if(FAILED(CoCreateInstance(CLSID_VideoMixingRenderer9,NULL,CLSCTX_INPROC_SERVER,IID_IBaseFilter,(void **)&g_filter)))
		return false;
	
	//add the VMR-9 to the filter graph
	if(FAILED(g_graph->AddFilter(g_filter, L"VMR9")))
		return false;
	
	//创建VMRFilterConfig9
	if(FAILED(g_filter->QueryInterface(IID_IVMRFilterConfig9, reinterpret_cast<void**>(&g_filterConfig))))
		return false;
	
	g_filterConfig->SetRenderingMode(VMR9Mode_Windowless);  //将其配置为WINDOWLESS模式
	
	//创建VMRWindowlessControl9
	if(FAILED(g_filter->QueryInterface(IID_IVMRWindowlessControl9, reinterpret_cast<void**>(&g_VMR))))
		return false;
	
	g_VMR->SetVideoClippingWindow(hwnd);    //指定要渲染的窗口为我们的应用程序
	
	//下面的三行描述我们的渲染窗口为整个应用程序客户区（整个配置过程适用于窗口模//式和全屏模式）
	RECT *clientRect = new RECT;
	::GetClientRect(hwnd, clientRect );

	if(FAILED(g_VMR->SetVideoPosition( NULL, clientRect )))
		return false;


	return true;
}
/**********************************/

void RefVideo()
{
	if(g_VMR!=NULL)
	{
		RECT *clientRect = new RECT;
		::GetClientRect(hwnd, clientRect );
		g_VMR->SetVideoPosition( NULL, clientRect );
	}
}

void releaseVideo()
{
	RELEASEEX(g_graph);    //
	RELEASEEX(g_sound);
 	RELEASEEX(g_source);
 	RELEASEEX(g_enumpins);
 	RELEASEEX(g_Graph2);
	RELEASEEX(pEvent);
 	RELEASEEX(g_mediaControl);
	RELEASEEX(g_VMR);           //
	RELEASEEX(g_filter);   //
// 	RELEASEEX(g_filterConfig);  //
	CoUninitialize();
}

IPin* GetPin(IBaseFilter *pFilter, PIN_DIRECTION PinDir)

{
	BOOL       bFound = FALSE;
	IEnumPins  *pEnum;
	IPin       *pPin;

	HRESULT hr = pFilter->EnumPins(&pEnum);  //搜索上流设备的PIN

	if (FAILED(hr))
		return NULL;

    while(pEnum->Next(1, &pPin, 0) == S_OK) 
	{
        PIN_DIRECTION PinDirThis;
        pPin->QueryDirection(&PinDirThis);
        if (bFound = (PinDir == PinDirThis))   //找到上流设备的输出PIN
            break;

        pPin->Release();
	}
	
	pEnum->Release();
	if(!bFound)MessageBox(NULL,L"Pin连接失败",L"Video播放失败",MB_OK);

    return (bFound ? pPin : NULL);  

}//该方法主要就是帮助g_Graph2->RenderEx方法和上流设备的媒体输出PIN进行对连

//以便将上流设备解码后的媒体信息流出到RENDER（应用程序创建的渲染器）


/**********************************/
/*Start playing the media*/
bool playVideo(LPCTSTR path)
{
	g_graph->AddSourceFilter(path, L"source", &g_source);  //将资源载入
	g_graph->RenderFile(path, 0);

	//创建IFilterGrahp2处理媒体流
	if(FAILED(g_graph->QueryInterface(IID_IFilterGraph2,reinterpret_cast<void**>(&g_Graph2))))
		return false;
	
	//指定RENDER渲染设备对上流设备的数据进行处理
	g_Graph2->RenderEx(GetPin(g_source,PINDIR_OUTPUT),AM_RENDEREX_RENDERTOEXISTINGRENDERERS,NULL);

	if(FAILED(g_graph->QueryInterface(IID_IMediaControl,reinterpret_cast<void**>(&g_mediaControl))))
		return false;

	g_graph->QueryInterface(IID_IMediaEvent, (void **)&pEvent);

	g_graph->AddSourceFilter(path, path, &g_sound);
	IPin *pPin = NULL;
	g_sound->FindPin(L"Output", &pPin);

	g_mediaControl->Run();

	return true;
}
/*************************/

void stopVideo()
{
	g_mediaControl->Stop();
}

bool VideoIsEnd()
{
	LONG eventCode =0, eventParam1 = 0, eventParam2 = 0;
	static BOOL bDone = FALSE;
	
	pEvent->GetEvent(&eventCode,&eventParam1,&eventParam2,0);
	pEvent->FreeEventParams(eventCode,eventParam1,eventParam2);

	switch(eventCode)
	{
	case EC_COMPLETE:
	case EC_END_OF_SEGMENT:
		{
			g_mediaControl->Stop();
			return true;
			break;
		}
	}
	return false;
}