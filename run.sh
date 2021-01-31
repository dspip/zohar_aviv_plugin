# -e GST_DEBUG="prointerlace:6,deinterlace:6"
xhost +
docker build -t test:zohar . && docker run -it --rm --network=host -v /tmp/.X11-unix:/tmp/.X11-unix -e DISPLAY=$DISPLAY test:zohar $@