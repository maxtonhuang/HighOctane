///******************************************************************************
//*
//*	\copyright
//*		All content(C) 2023/2024 DigiPen Institute of Technology Singapore.
//*		All rights reserved. Reproduction or disclosure of this file or its
//*		contents without the prior written consent of DigiPen Institute of
//*		Technology is prohibited.
//*
//* *****************************************************************************
//*
//*	@file		ImGuiGraph.cpp
//*
//*	@author		Koh Wen Yuan
//*
//*	@email		k.wenyuan\@digipen.edu
//*
//*	@course		CSD 2401 - Software Engineering Project 3
//*				CSD 2451 - Software Engineering Project 4
//*
//*	@section	Section A
//*
//*	@date		23 September 2023
//*
//* *****************************************************************************
//*
//*	@brief		Graph window for ImGui
//*
//*	This file contains all the definition of the functions for the graph
//*
//* WARNING THIS FILE SHOULD NOT BE UNCOMMENTED, STILL UPDATING IN PROGRESS
//* WILL BE IMPLEMENTED IN THE FUTURE
//*
//******************************************************************************/
//
//#include "ImGuiGraph.h"
//#include "enginecore.h"
//
//#if ENABLE_DEBUG_PROFILE
//extern std::vector<std::pair<std::shared_ptr<System>, std::string>> systemList;
//extern DebugProfiling debugSysProfile;
//
//
//int bar_data[11] = { 20, 45, 70, 15, 90, 30, 50, 80, 35, 65, 25 };
//float x_data[10] = { 0.2f, 0.5f, 0.8f, 0.1f, 0.9f, 0.3f, 0.6f, 0.7f, 0.4f, 0.25f };
//float y_data[10] = { 0.4f, 0.1f, 0.7f, 0.9f, 0.2f, 0.6f, 0.3f, 0.8f, 0.5f, 0.75f };
//struct scrollingbuffer {
//    int maxsize;
//    int offset;
//    imvector<imvec2> data;
//    scrollingbuffer(int max_size = 2000) {
//        maxsize = max_size;
//        offset = 0;
//        data.reserve(maxsize);
//    }
//    void addpoint(float x, float y) {
//        if (data.size() < maxsize)
//            data.push_back(imvec2(x, y));
//        else {
//            data[offset] = imvec2(x, y);
//            offset = (offset + 1) % maxsize;
//        }
//    }
//    void erase() {
//        if (data.size() > 0) {
//            data.shrink(0);
//            offset = 0;
//        }
//    }
//};
//
//// utility structure for realtime plot
//struct rollingbuffer {
//    float span;
//    imvector<imvec2> data;
//    rollingbuffer() {
//        span = 10.0f;
//        data.reserve(2000);
//    }
//    void addpoint(float x, float y) {
//        float xmod = fmodf(x, span);
//        if (!data.empty() && xmod < data.back().x)
//            data.shrink(0);
//        data.push_back(imvec2(xmod, y));
//    }
//};
//
///*!
//* \brief Init the graph window
//*
//* This function is responsible for initialising the graph window
//* It will be called in ImGuiManager.cpp
//*
//*/
//
//void InitGraph() {
//
//    // Setup Dear ImGui context
//    IMGUI_CHECKVERSION();
//    ImGuiIO& io = ImGui::GetIO(); (void)io;
//    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
//
//    // Setup Dear ImGui style
//    ImGui::StyleColorsDark();
//
//}
//
///*!
//* \brief Update the graph window
//*
//* This function is responsible for updating the graph window
//* It will be called in ImGuiManager.cpp
//*
//*/
//void UpdateGraph() {
//    ImGui::Begin("Graph Window");
//        ImGui::BulletText("Move your mouse to change the data!");
//        ImGui::BulletText("This example assumes 60 FPS. Higher FPS requires larger buffer size.");
//
//        static ScrollingBuffer sdata1, sdata2;
//        static RollingBuffer   rdata1, rdata2;
//        ImVec2 mouse = ImGui::GetMousePos();
//        static float t = 0;
//        t += ImGui::GetIO().DeltaTime;
//        sdata1.AddPoint(t, mouse.x * 0.0005f);
//        rdata1.AddPoint(t, mouse.x * 0.0005f);
//        sdata2.AddPoint(t, mouse.y * 0.0005f);
//        rdata2.AddPoint(t, mouse.y * 0.0005f);
//
//        static float history = 10.0f;
//        ImGui::SliderFloat("History", &history, 1, 30, "%.1f s");
//        rdata1.Span = history;
//        rdata2.Span = history;
//
//        static ImPlotAxisFlags flags = ImPlotAxisFlags_NoTickLabels;
//
//        if (ImPlot::BeginPlot("##Scrolling", ImVec2(-1, 150))) {
//            ImPlot::SetupAxes(nullptr, nullptr, flags, flags);
//            ImPlot::SetupAxisLimits(ImAxis_X1, t - history, t, ImGuiCond_Always);
//            ImPlot::SetupAxisLimits(ImAxis_Y1, 0, 1);
//            ImPlot::SetNextFillStyle(IMPLOT_AUTO_COL, 0.5f);
//            ImPlot::PlotShaded("Mouse X", &sdata1.Data[0].x, &sdata1.Data[0].y, sdata1.Data.size(), -INFINITY, 0, sdata1.Offset, 2 * sizeof(float));
//            ImPlot::PlotLine("Mouse Y", &sdata2.Data[0].x, &sdata2.Data[0].y, sdata2.Data.size(), 0, sdata2.Offset, 2 * sizeof(float));
//            ImPlot::EndPlot();
//        }
//        if (ImPlot::BeginPlot("##Rolling", ImVec2(-1, 150))) {
//            ImPlot::SetupAxes(nullptr, nullptr, flags, flags);
//            ImPlot::SetupAxisLimits(ImAxis_X1, 0, history, ImGuiCond_Always);
//            ImPlot::SetupAxisLimits(ImAxis_Y1, 0, 1);
//            ImPlot::PlotLine("Mouse X", &rdata1.Data[0].x, &rdata1.Data[0].y, rdata1.Data.size(), 0, 0, 2 * sizeof(float));
//            ImPlot::PlotLine("Mouse Y", &rdata2.Data[0].x, &rdata2.Data[0].y, rdata2.Data.size(), 0, 0, 2 * sizeof(float));
//            ImPlot::EndPlot();
//        }
//        ImPlot::ShowDemoWindow();
//    ImGui::End();
//}
//
//#endif