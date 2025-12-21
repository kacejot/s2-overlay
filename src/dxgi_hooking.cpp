#include "dxgi_hooking.h"

#include <dxgi.h>
#include <imgui.h>
#include <backends/imgui_impl_dx12.h>
#include <backends/imgui_impl_win32.h>

HRESULT create_dxgi_factory_hook::operator()(create_dxgi_factory_t original, const IID& riid, void** ppFactory)
{
    LOG(" --- create factory");
    HRESULT hr = original(riid, ppFactory);
    if (FAILED(hr) || ppFactory == nullptr || *ppFactory == nullptr)
        return hr;

    void* factory = *ppFactory;
    void** vtable = *(void***)(factory);
    m_master.try_add_hook<IDXGI_FACTORY1_CREATE_SWAP_CHAIN>(
        vtable[10],
		m_master.idxgi_factory1_create_swap_chain);

    m_master.create_f2_hooks((IUnknown*)*ppFactory);
    return hr;
}

HRESULT create_dxgi_factory1_hook::operator()(create_dxgi_factory1_t original, const IID& riid, void** ppFactory)
{
    LOG(" --- create factory1");
    HRESULT hr = original(riid, ppFactory);
    if (FAILED(hr) || ppFactory == nullptr || *ppFactory == nullptr)
        return hr;

    void* factory = *ppFactory;
    void** vtable = *(void***)(factory);

    m_master.try_add_hook<IDXGI_FACTORY1_CREATE_SWAP_CHAIN>(
		vtable[10],
		m_master.idxgi_factory1_create_swap_chain);

    m_master.create_f2_hooks((IUnknown*)*ppFactory);
    return hr;
}


HRESULT create_dxgi_factory2_hook::operator()(create_dxgi_factory2_t original, uint32_t flags, const IID& riid, void** ppFactory)
{
    LOG(" --- create factory2");
    HRESULT hr = original(flags, riid, ppFactory);

    if (FAILED(hr) || !ppFactory || !*ppFactory)
        return hr;

    m_master.create_f2_hooks((IUnknown*)(*ppFactory));
    return hr;
}

HRESULT __fastcall idxgi_factory_create_swap_chain_hook::operator()(idxgi_factory_create_swap_chain_t original, uintptr_t self, IUnknown* pDevice, DXGI_SWAP_CHAIN_DESC* pDesc, IDXGISwapChain** ppSwapChain)
{
    LOG(" --- factory -> create swap chain");
    auto hr = original(self, pDevice, pDesc, ppSwapChain);

    if (SUCCEEDED(hr) && ppSwapChain && *ppSwapChain)
    {
        m_master.m_dx12.command_queue = (ID3D12CommandQueue*)pDevice;

        IDXGISwapChain* sc = *ppSwapChain;
        void** vtable = *(void***)(sc);

        m_master.try_add_hook<DXGI_SWAP_CHAIN_PRESENT>(
            vtable[8],
			m_master.dxgi_swap_chain_present);
    };

    return hr;
}

HRESULT __fastcall idxgi_factory1_create_swap_chain_hook::operator()(idxgi_factory_create_swap_chain_t original, uintptr_t self, IUnknown* pDevice, DXGI_SWAP_CHAIN_DESC* pDesc, IDXGISwapChain** ppSwapChain)
{
    LOG(" --- factory1 -> create swap chain");
    auto hr = original(self, pDevice, pDesc, ppSwapChain);

    if (SUCCEEDED(hr) && ppSwapChain && *ppSwapChain)
    {
        m_master.m_dx12.command_queue = (ID3D12CommandQueue*)pDevice;

        IDXGISwapChain* sc = *ppSwapChain;
        void** vtable = *(void***)(sc);

        m_master.try_add_hook<DXGI_SWAP_CHAIN_PRESENT>(
            vtable[8],
			m_master.dxgi_swap_chain_present);
    }
    return hr;
}

HRESULT __fastcall idxgi_factory2_create_swap_chain_hook::operator()(
    idxgi_factory_create_swap_chain_t original,
    uintptr_t self,
    IUnknown* pDevice,
    DXGI_SWAP_CHAIN_DESC* pDesc,
    IDXGISwapChain** ppSwapChain)
{
    LOG(" --- factory2 -> create swap chain");
    auto hr = original(self, pDevice, pDesc, ppSwapChain);

    if (SUCCEEDED(hr) && ppSwapChain && *ppSwapChain)
    {
        m_master.m_dx12.command_queue = (ID3D12CommandQueue*)pDevice;

        IDXGISwapChain* sc = *ppSwapChain;
        void** vtable = *(void***)(sc);

        m_master.try_add_hook<DXGI_SWAP_CHAIN_PRESENT>(
            vtable[8],
			m_master.dxgi_swap_chain_present);
    }
    return hr;
}

HRESULT __fastcall idxgi_factory2_create_swap_chain_for_hwnd_hook::operator()(
    idxgi_factory2_create_swap_chain_for_hwnd_t original,
    IUnknown* pDevice,
    HWND hWnd,
    const DXGI_SWAP_CHAIN_DESC1* pDesc,
    const DXGI_SWAP_CHAIN_FULLSCREEN_DESC* pFullscreenDesc,
    IDXGIOutput* pRestrictToOutput,
    IDXGISwapChain1** ppSwapChain)
{
    LOG(" --- factory2 -> create swap chain for hwnd");
    auto hr = original(pDevice, hWnd, pDesc, pFullscreenDesc, pRestrictToOutput, ppSwapChain);
    
    if (SUCCEEDED(hr) && ppSwapChain && *ppSwapChain)
    {
        m_master.m_dx12.command_queue = (ID3D12CommandQueue*)pDevice;

        IDXGISwapChain* sc = *ppSwapChain;
        void** vtable = *(void***)(sc);
        void* present_addr = vtable[8];

		if (!m_master.try_add_hook<DXGI_SWAP_CHAIN_PRESENT>(present_addr, m_master.dxgi_swap_chain_present))
            return hr;
    }

    return hr;
}

HRESULT __fastcall idxgi_factory2_create_swap_chain_for_core_window_hook::operator()(
    idxgi_factory2_create_swap_chain_for_core_window_t original,
    IUnknown* pDevice,
    IUnknown* pWindow,
    const DXGI_SWAP_CHAIN_DESC1* pDesc,
    IDXGIOutput* pRestrictToOutput,
    IDXGISwapChain1** ppSwapChain)
{
    LOG(" --- factory2 -> create swap chain for core window");
    auto hr = original(pDevice, pWindow, pDesc, pRestrictToOutput, ppSwapChain);

    if (SUCCEEDED(hr) && ppSwapChain && *ppSwapChain)
    {
        m_master.m_dx12.command_queue = (ID3D12CommandQueue*)pDevice;

        IDXGISwapChain* sc = *ppSwapChain;
        void** vtable = *(void***)(sc);

        m_master.try_add_hook<DXGI_SWAP_CHAIN_PRESENT>(
            vtable[8],
			m_master.dxgi_swap_chain_present);
    }

    return hr;
}

HRESULT __fastcall idxgi_factory2_create_swap_chain_for_composition_hook::operator()(
    idxgi_factory2_create_swap_chain_for_composition_t original,
    IUnknown* pDevice,
    const DXGI_SWAP_CHAIN_DESC1* pDesc,
    IDXGIOutput* pRestrictToOutput,
    IDXGISwapChain1** ppSwapChain)
{
    LOG(" --- factory2 -> create swap chain for composition");
    auto hr = original(pDevice, pDesc, pRestrictToOutput, ppSwapChain);

    if (SUCCEEDED(hr) && ppSwapChain && *ppSwapChain)
    {
        m_master.m_dx12.command_queue = (ID3D12CommandQueue*)pDevice;

        IDXGISwapChain* sc = *ppSwapChain;
        void** vtable = *(void***)(sc);

        m_master.try_add_hook<DXGI_SWAP_CHAIN_PRESENT>(
            vtable[8],
			m_master.dxgi_swap_chain_present);
    }

    return hr;
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

HRESULT dxgi_swap_chain_present_hook::operator()(dxgi_swap_chain_present_t original, IDXGISwapChain3* self, UINT SyncInterval, UINT Flags)
{
    auto& dx12 = m_master.m_dx12;

    if (!dx12.initialized && dx12.command_queue)
    {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.MouseDrawCursor = true;

        LOG(" --- Initializing DX12 ImGui");

        DXGI_SWAP_CHAIN_DESC swap_chain_desc;
        self->GetDevice(__uuidof(ID3D12Device), (void**)&dx12.device);
        self->GetDesc(&swap_chain_desc);
        self->GetHwnd(&dx12.hwnd);
        swap_chain_desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
        swap_chain_desc.OutputWindow = dx12.hwnd;
        swap_chain_desc.Windowed = ((GetWindowLongPtr(dx12.hwnd, GWL_STYLE) & WS_POPUP) != 0) ? false : true;

        dx12.buffer_count = swap_chain_desc.BufferCount;
        dx12.frame_ctx = (frame_context*)calloc(dx12.buffer_count, sizeof(frame_context));

        D3D12_DESCRIPTOR_HEAP_DESC srv_desc = {};
        srv_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        srv_desc.NumDescriptors = dx12.buffer_count;
        srv_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

        dx12.device->CreateDescriptorHeap(&srv_desc, IID_PPV_ARGS(&dx12.srv_heap));

        ID3D12CommandAllocator* allocator;
        dx12.device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&allocator));
        for (size_t i = 0; i < dx12.buffer_count; i++) {
            dx12.frame_ctx[i].command_allocator = allocator;
        }

        dx12.device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, allocator, NULL, IID_PPV_ARGS(&dx12.command_list));

        LOG(" --- 4");
        D3D12_DESCRIPTOR_HEAP_DESC rtv_desc = {};
        rtv_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
        rtv_desc.NumDescriptors = dx12.buffer_count;
        rtv_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        rtv_desc.NodeMask = 1;

        LOG(" --- 5");
        dx12.device->CreateDescriptorHeap(&rtv_desc, IID_PPV_ARGS(&dx12.rtv_heap));

        LOG(" --- 6");
        UINT rtv_size = dx12.device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
        D3D12_CPU_DESCRIPTOR_HANDLE rtv_handle = dx12.rtv_heap->GetCPUDescriptorHandleForHeapStart();

        LOG(" --- 7");
        for (UINT i = 0; i < dx12.buffer_count; i++)
        {
            ID3D12Resource* back_buffer = nullptr;
            dx12.frame_ctx[i].rtv_handle = rtv_handle;
            self->GetBuffer(i, IID_PPV_ARGS(&back_buffer));
            dx12.device->CreateRenderTargetView(back_buffer, nullptr, rtv_handle);
            dx12.frame_ctx[i].back_buffer = back_buffer;
            rtv_handle.ptr += rtv_size;
        }

        LOG(" --- 8");
        ImGui_ImplWin32_Init(dx12.hwnd);

        LOG(" --- 9");
        ImGui_ImplDX12_Init(
            dx12.device,
            dx12.buffer_count,
            DXGI_FORMAT_R8G8B8A8_UNORM,
            dx12.srv_heap,
            dx12.srv_heap->GetCPUDescriptorHandleForHeapStart(),
            dx12.srv_heap->GetGPUDescriptorHandleForHeapStart()
        );

        LOG(" --- 10");
        io.Fonts->Build();
        ImGui_ImplDX12_CreateDeviceObjects();

        LOG(" --- 11");
        void* wnd_proc = (void*)GetWindowLongPtrW(dx12.hwnd, GWLP_WNDPROC);
        if (wnd_proc && !m_master.m_hk.has_hook(WND_PROC))
        {
            m_master.m_hk.add_hook<WND_PROC>(
                (uintptr_t)wnd_proc,
                [&dx12](WNDPROC orig, HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) -> LRESULT {
                    if (wParam == VK_OEM_MINUS)
                    {
                        dx12.show_ui = !dx12.show_ui;
                        return true;
                    }
                    if (dx12.show_ui && ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
                        return true;
                    return orig(hWnd, msg, wParam, lParam);
                });
        }

        dx12.initialized = true;
        LOG(" --- DX12 ImGui initialized successfully");
    }

    if (dx12.show_ui && dx12.command_queue && dx12.initialized)
    {
        ImGui_ImplDX12_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        ImGui::ShowDemoWindow();

        UINT frame_idx = self->GetCurrentBackBufferIndex();
        frame_context& ctx = dx12.frame_ctx[frame_idx];
        ctx.command_allocator->Reset();

        D3D12_RESOURCE_BARRIER barrier = {};
        barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
        barrier.Transition.pResource = ctx.back_buffer;
        barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
        barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
        barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;

        dx12.command_list->Reset(ctx.command_allocator, nullptr);
        dx12.command_list->ResourceBarrier(1, &barrier);
        dx12.command_list->OMSetRenderTargets(1, &ctx.rtv_handle, FALSE, nullptr);
        dx12.command_list->SetDescriptorHeaps(1, &dx12.srv_heap);

        ImGui::Render();
        ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), dx12.command_list);

        barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
        barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;

        dx12.command_list->ResourceBarrier(1, &barrier);
        dx12.command_list->Close();

        dx12.command_queue->ExecuteCommandLists(1, reinterpret_cast<ID3D12CommandList* const*>(&dx12.command_list));
    }

    return original(self, SyncInterval, Flags);
}

void dxgi_hooking::create_f2_hooks(IUnknown* factory)
{
    if (!factory)
        return;

    IDXGIFactory2* f2 = nullptr;
    if (FAILED(factory->QueryInterface(__uuidof(IDXGIFactory2), (void**)&f2)))
    {
        LOG("Failed to query interface");
        return;
    }

    void** vtable = *(void***)(f2);

    void* target = vtable[10];
    if (check(target, IDXGI_FACTORY2_CREATE_SWAP_CHAIN))
        m_hk.add_hook<IDXGI_FACTORY2_CREATE_SWAP_CHAIN>((uintptr_t)target, idxgi_factory2_create_swap_chain);

    target = vtable[15];
    if (check(target, IDXGI_FACTORY2_CREATE_SWAP_CHAIN_FOR_HWND))
        m_hk.add_hook<IDXGI_FACTORY2_CREATE_SWAP_CHAIN_FOR_HWND>((uintptr_t)target, idxgi_factory2_create_swap_chain_for_hwnd);

    target = vtable[16];
    if (check(target, IDXGI_FACTORY2_CREATE_SWAP_CHAIN_FOR_CORE_WINDOW))
        m_hk.add_hook<IDXGI_FACTORY2_CREATE_SWAP_CHAIN_FOR_CORE_WINDOW>((uintptr_t)target, idxgi_factory2_create_swap_chain_for_core_window);

    target = vtable[24];
    if (check(target, IDXGI_FACTORY2_CREATE_SWAP_CHAIN_FOR_COMPOSITION))
        m_hk.add_hook<IDXGI_FACTORY2_CREATE_SWAP_CHAIN_FOR_COMPOSITION>((uintptr_t)target, idxgi_factory2_create_swap_chain_for_composition);

    f2->Release();
}

bool dxgi_hooking::check(void* addr, function_id_t id)
{
    if (!is_inside_module(addr))
    {
        LOG("{} is not inside dxgi.dll module - skipping", (uint32_t)id);
        return false;
    }

    if (m_hk.has_hook(id))
    {
        return false;
    }

    return true;
}

void dxgi_hooking::init()
{
    if (NULL == m_dxgi_handle)
    {
        LOG("Failed to get dxgi.dll handle!");
        return;
    }

    LOG("dxgi.dll handle is {:x}", (uintptr_t)m_dxgi_handle);

	if (!try_add_hook_by_name<CREATE_DXGI_FACTORY, create_dxgi_factory_t>("CreateDXGIFactory", create_dxgi_factory))
        return;

	if (!try_add_hook_by_name<CREATE_DXGI_FACTORY1, create_dxgi_factory1_t >("CreateDXGIFactory1", create_dxgi_factory1))
        return;

	if (!try_add_hook_by_name<CREATE_DXGI_FACTORY2, create_dxgi_factory2_t>("CreateDXGIFactory2", create_dxgi_factory2))
        return;
}

bool dxgi_hooking::is_inside_module(void* addr)
{
    HMODULE actual_module = nullptr;
    GetModuleHandleExA(
        GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
        reinterpret_cast<LPCSTR>(addr),
        &actual_module
    );

    if (m_dxgi_handle && actual_module && m_dxgi_handle == actual_module)
        return true;

    char filename[MAX_PATH]{};
    GetModuleFileNameA(actual_module, filename, MAX_PATH);

    LOG("Address expected to be in {:x} but found in {}", (uintptr_t)m_dxgi_handle, filename);
    return false;
}
