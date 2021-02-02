#pragma once

#include <gst/app/app.h>
#include <gst/gst.h>
#include <gst/video/video.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <iostream>
#include <sstream>
#include <string>

class App {
   private:
    GMainLoop *loop = NULL;
    GstElement *current_pipeline = NULL;

    void start_pipeline();
    void stop_pipeline();
    void pause_pipeline();
    void resume_pipeline();
    void create_pipeline(std::string pipeline);
    static gboolean bus_callback(GstBus *bus, GstMessage *msg, gpointer data);
    static GstFlowReturn on_new_sample(GstElement *element, gpointer user_data);

    void on_video_frame(GstVideoFrame *frame);

   public:
    App();
    virtual ~App();
    void init();
    void play_screen();
    void play_get_video_packets();
    void start();
};