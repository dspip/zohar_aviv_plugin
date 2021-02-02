FROM ubuntu:rolling
ENV DEBIAN_FRONTEND=noninteractive
RUN apt-get update

# GStreamer + libsoup
RUN apt-get install -y gstreamer1.0-tools
RUN apt-get install -y gstreamer1.0-nice
RUN apt-get install -y gstreamer1.0-plugins-bad
RUN apt-get install -y libgstreamer-plugins-bad1.0-dev
# RUN apt-get install -y libgstreamer1.0-dev
RUN apt-get install -y gstreamer1.0-plugins-ugly
RUN apt-get install -y gstreamer1.0-plugins-good
# RUN apt-get install -y gstreamer1.0-rtsp
RUN apt-get install -y libgstreamer1.0-dev
RUN apt-get install -y libglib2.0-dev
RUN apt-get install -y libsoup2.4-dev
RUN apt-get install -y libjson-glib-dev
RUN apt-get install -y cmake
RUN apt-get install -y git
RUN apt-get install -y ninja-build
RUN apt-get install -y zip
RUN apt-get install -y ubuntu-restricted-extras
RUN apt-get install -y ffmpeg
RUN apt-get install -y wget
RUN apt-get install -y curl
RUN apt-get install -y gcc
RUN apt-get install -y g++
RUN apt-get autoremove -y

RUN mkdir -p /app/
WORKDIR /app/

COPY . .

RUN mkdir build && cd build \ 
    && cmake .. -DCMAKE_TOOLCHAIN_FILE=$CMAKE_TOOLCHAIN_FILE -DCMAKE_BUILD_TYPE:STRING=Release \
    && cmake --build . --config Release --target all -- -j 18 \
    && make install

CMD ./build/App

# CMD gst-launch-1.0 filesrc location="./videos/h264FirstSensorD.pcap" ! pcapparse dst-ip=10.201.41.100 dst-port=5002 ! rtph264pass ! h264parse ! avdec_h264 ! autovideosink

# CMD gst-launch-1.0 udpsrc address=239.3.0.1 port=6001 ! capsfilter caps="application/x-rtp, encoding-name=H264, clock-rate=90000" ! rtpjitterbuffer ! rtph264pass ! h264parse ! avdec_h264 ! autovideosink -v

# CMD gst-launch-1.0 udpsrc address=239.3.0.1 port=6001 ! rtph264pass ! h264parse ! avdec_h264 ! autovideosink