#include <App.h>

gboolean App::bus_callback(GstBus *bus, GstMessage *msg, gpointer data) {
    App *self = (App *)data;
    GError *err = NULL;
    gchar *debug = NULL;
    switch (GST_MESSAGE_TYPE(msg)) {
        case GST_MESSAGE_EOS:
            g_print("got EOS\n");
            break;
        case GST_MESSAGE_WARNING:
            gst_message_parse_warning(msg, &err, &debug);
            g_print("[WARNING] %s\n%s\n", err->message, debug);
            break;
        case GST_MESSAGE_ERROR:
            gst_message_parse_error(msg, &err, &debug);
            g_print("[ERROR] %s\n%s\n", err->message, debug);
        default:
            break;
    }
    return TRUE;
}

App::App(){};
App::~App() {
    stop_pipeline();
};

void App::start_pipeline() {
    if (this->current_pipeline != NULL) {
        std::cout << "here" << std::endl;
        gst_element_set_state(this->current_pipeline, GST_STATE_PLAYING);
    }
}

void App::stop_pipeline() {
    if (this->current_pipeline != NULL) {
        gst_element_set_state(this->current_pipeline, GST_STATE_NULL);
        this->current_pipeline = NULL;
    }
}

void App::pause_pipeline() {
    if (this->current_pipeline != NULL) {
        gst_element_set_state(this->current_pipeline, GST_STATE_PAUSED);
    }
}

void App::resume_pipeline() {
    if (this->current_pipeline != NULL) {
        if (gst_element_set_state(this->current_pipeline, GST_STATE_PLAYING) != GST_STATE_CHANGE_FAILURE) {
            // this->running = true;
        }
    }
}

void App::create_pipeline(std::string pipe) {
    GError *e = NULL;
    this->current_pipeline = gst_parse_launch(pipe.c_str(), &e);
    if (e != NULL || this->current_pipeline == NULL) {
        std::cout << "[ERROR] Failed to run pipeline: \n ~~~ " << pipe << "\n"
                  << "[Error]: " << e->message << std::endl;
        throw std::runtime_error(e->message);
    }
    std::cout << "Running pipeline: \n ~~~ " << pipe << std::endl;
    GstBus *bus = gst_element_get_bus(this->current_pipeline);
    gst_bus_add_watch(bus, App::bus_callback, this);
    gst_object_unref(bus);
}

void App::play_screen() {
    std::stringstream s;
    s
        << "filesrc location=\"videos/h264FirstSensorD.pcap\" ! pcapparse dst-port=5002 ! "
        // << "udpsrc address=0.0.0.0 port=5002 ! "
        << "capsfilter caps=\"application/x-rtp, encoding-name=H264, clock-rate=90000\" ! "
        << "rtpjitterbuffer latency=0 ! "
        << "rtph264pass ! "
        << "h264parse ! "
        << "avdec_h264 ! "
        << "videocrop top=0 left=0 right=320 bottom=240 ! "
        << "autovideosink ";

    create_pipeline(s.str());
    start_pipeline();
}

void print_frame_plane_info(GstVideoFrame *frame, int plane) {
    const guint8 *data = (guint8 *)GST_VIDEO_FRAME_PLANE_DATA(frame, plane);
    const guint width = GST_VIDEO_FRAME_COMP_WIDTH(frame, plane);
    const guint height = GST_VIDEO_FRAME_COMP_HEIGHT(frame, plane);
    const guint stride = GST_VIDEO_FRAME_COMP_STRIDE(frame, plane);
    const guint pixel_stride = GST_VIDEO_FRAME_COMP_PSTRIDE(frame, plane);

    std::cout << "plane #" << plane << ": \n\t"
              << "width: " << width << "\n\t"
              << "height: " << height << "\n\t"
              << "stride: " << stride << "\n\t"
              << "pixel_stride: " << pixel_stride << "\n";
}

void App::on_video_frame(GstVideoFrame *frame) {
    // planes [y,u,v] correspond to plane indexes [0,1,2]
    const uint planes = GST_VIDEO_FRAME_N_PLANES(frame);
    for (uint plane = 0; plane < planes; plane++) {
        print_frame_plane_info(frame, plane);
    }
    // const guint8 *y = (guint8 *)GST_VIDEO_FRAME_PLANE_DATA(frame, 0);
    // const guint8 *u = (guint8 *)GST_VIDEO_FRAME_PLANE_DATA(frame, 1);
    // const guint8 *v = (guint8 *)GST_VIDEO_FRAME_PLANE_DATA(frame, 2);
}

GstFlowReturn App::on_new_sample(GstElement *element, gpointer user_data) {
    App *self = (App *)user_data;
    GstAppSink *appsink = (GstAppSink *)element;

    GstSample *sample = gst_app_sink_pull_sample(appsink);
    if (sample == NULL) return GST_FLOW_ERROR;

    GstCaps *caps = gst_sample_get_caps(sample);
    GstBuffer *buffer = gst_sample_get_buffer(sample);
    if (caps == NULL || buffer == NULL) return GST_FLOW_ERROR;

    GstVideoInfo info;
    if (!gst_video_info_from_caps(&info, caps)) return GST_FLOW_ERROR;

    GstVideoFrame frame;
    if (!gst_video_frame_map(&frame, &info, buffer, GST_MAP_READ)) return GST_FLOW_ERROR;

    self->on_video_frame(&frame);

    gst_video_frame_unmap(&frame);
    gst_sample_unref(sample);
    return GST_FLOW_OK;
}

void App::play_get_video_packets() {
    std::stringstream s;
    s
        << "filesrc location=\"videos/h264FirstSensorD.pcap\" ! pcapparse dst-port=5002 ! "
        // << "udpsrc address=0.0.0.0 port=5002 ! "
        << "capsfilter caps=\"application/x-rtp, encoding-name=H264, clock-rate=90000\" ! "
        << "rtpjitterbuffer latency=0 ! "
        << "rtph264pass ! "
        << "h264parse ! "
        << "avdec_h264 ! "
        << "videocrop top=0 left=0 right=320 bottom=240 ! "
        << "appsink name=appsink emit-signals=true";

    create_pipeline(s.str());
    GstElement *appsink = gst_bin_get_by_name(GST_BIN(this->current_pipeline), "appsink");
    g_signal_connect(appsink, "new_sample", G_CALLBACK(App::on_new_sample), this);
    start_pipeline();
}

void App::start() {
    g_main_loop_run(this->loop);
}

void App::init() {
    gst_init(NULL, NULL);
    this->loop = g_main_loop_new(NULL, false);
}

int main(int argc, char *argv[]) {
    App app;
    app.init();
    app.play_screen();
    // app.play_get_video_packets();
    app.start();
    return 0;
}