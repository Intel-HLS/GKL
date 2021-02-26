FROM centos:7

RUN yum install -y \
  java-1.8.0-openjdk-devel \
  git \
  cmake \
  patch \
  libtool \
  automake \
  zlib-devel \
  centos-release-scl \
  scl-utils \
  epel-release \
  && yum install -y devtoolset-7 yasm

ENV JAVA_HOME=/usr/lib/jvm/jre
