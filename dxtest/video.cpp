#include "video.h"
#define RELEASEEX(x) if(x){x->Release();x=NULL;}

IGraphBuilder*            g_graph;        //FILTER������
IBaseFilter*              g_filter;         //���ڴ���VMR9
IBaseFilter*              g_source;        //������ý�����ݴ���
IEnumPins*                g_enumpins;     //���ڻ�ȡPIN
IFilterGraph2*            g_Graph2;             //������Ƶ����Ⱦ����
IMediaControl*            g_mediaControl;   //ý�����
IMediaEvent*              pEvent;
IVMRFilterConfig9*        g_filterConfig;     //VMR9����
IVMRWindowlessControl9*   g_VMR;         //VMR9��WINDOWEDģʽ����
extern HWND               hwnd;                     //��ǰ���ھ��
IBaseFilter*        g_sound;


/***********************************/
/*Initialization of the variables via calls to their respective interfaces*/
bool initVideo(void)
{
	//����FilterGraphManager
	if(FAILED(CoCreateInstance(CLSID_FilterGraph,NULL,CLSCTX_INPROC_SERVER,IID_IGraphBuilder,(void **)&g_graph)))
		return false;

	//����FilterGraph(VMR9)
	if(FAILED(CoCreateInstance(CLSID_VideoMixingRenderer9,NULL,CLSCTX_INPROC_SERVER,IID_IBaseFilter,(void **)&g_filter)))
		return false;
	
	//add the VMR-9 to the filter graph
	if(FAILED(g_graph->AddFilter(g_filter, L"VMR9")))
		return false;
	
	//����VMRFilterConfig9
	if(FAILED(g_filter->QueryInterface(IID_IVMRFilterConfig9, reinterpret_cast<void**>(&g_filterConfig))))
		return false;
	
	g_filterConfig->SetRenderingMode(VMR9Mode_Windowless);  //��������ΪWINDOWLESSģʽ
	
	//����VMRWindowlessControl9
	if(FAILED(g_filter->QueryInterface(IID_IVMRWindowlessControl9, reinterpret_cast<void**>(&g_VMR))))
		return false;
	
	g_VMR->SetVideoClippingWindow(hwnd);    //ָ��Ҫ��Ⱦ�Ĵ���Ϊ���ǵ�Ӧ�ó���
	
	//����������������ǵ���Ⱦ����Ϊ����Ӧ�ó���ͻ������������ù��������ڴ���ģ//ʽ��ȫ��ģʽ��
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

	HRESULT hr = pFilter->EnumPins(&pEnum);  //���������豸��PIN

	if (FAILED(hr))
		return NULL;

    while(pEnum->Next(1, &pPin, 0) == S_OK) 
	{
        PIN_DIRECTION PinDirThis;
        pPin->QueryDirection(&PinDirThis);
        if (bFound = (PinDir == PinDirThis))   //�ҵ������豸�����PIN
            break;

        pPin->Release();
	}
	
	pEnum->Release();
	if(!bFound)MessageBox(NULL,L"Pin����ʧ��",L"Video����ʧ��",MB_OK);

    return (bFound ? pPin : NULL);  

}//�÷�����Ҫ���ǰ���g_Graph2->RenderEx�����������豸��ý�����PIN���ж���

//�Ա㽫�����豸������ý����Ϣ������RENDER��Ӧ�ó��򴴽�����Ⱦ����


/**********************************/
/*Start playing the media*/
bool playVideo(LPCTSTR path)
{
	g_graph->AddSourceFilter(path, L"source", &g_source);  //����Դ����
	g_graph->RenderFile(path, 0);

	//����IFilterGrahp2����ý����
	if(FAILED(g_graph->QueryInterface(IID_IFilterGraph2,reinterpret_cast<void**>(&g_Graph2))))
		return false;
	
	//ָ��RENDER��Ⱦ�豸�������豸�����ݽ��д���
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