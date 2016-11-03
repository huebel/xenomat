FROM lpenz/debian-squeeze-i386:latest

MAINTAINER Peter Hübel "https://github.com/huebel"

ENV DEBIAN_FRONTEND noninteractive

ENV LANG en_US.UTF-8
ENV LANGUAGE en_US.UTF-8
ENV LC_ALL en_US.UTF-8
ENV HOME /root
ENV LD_LIBRARY_PATH /opt/xeno/lib
ENV LIB_PATH http://192.168.99.100/download 

# Prepare
RUN \
  mkdir -p /opt/xeno/bin && \
  mkdir -p /opt/xeno/content/js && \
  mkdir -p /opt/xeno/lib && \
  cd /opt/xeno/lib && \
  wget $LIB_PATH/lib/libboost_chrono.so.1.48.0 && \
  wget $LIB_PATH/lib/libboost_system.so.1.48.0 && \
  wget $LIB_PATH/lib/libboost_thread.so.1.48.0 && \
  echo "Done!"
    
# Copy xenomat
ADD ./bin/debian/netropy /opt/xeno/bin/xenomat
ADD ./examples/blueprint.xml /opt/xeno/content/blueprint.xml
ADD ./examples/blueprint.html /opt/xeno/content/blueprint.html
ADD ./examples/blueprint.css /opt/xeno/content/blueprint.css
ADD ./examples/blueprint.js /opt/xeno/content/blueprint.js
ADD ./examples/js/jquery-1.9.1.min.js /opt/xeno/content/js/jquery-1.9.1.min.js
ADD ./examples/js/d3.min.js /opt/xeno/content/js/d3.min.js
ADD ./examples/js/interact.js /opt/xeno/content/js/interact.js

# Fix permissions
RUN chmod 755 /opt/xeno/bin/xenomat

# Export default XENO port
EXPOSE 8901

# default command
CMD ["/opt/xeno/bin/xenomat", "-d", "/opt/xeno/content/", "/opt/xeno/content/blueprint.xml"]
