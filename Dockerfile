FROM registry.access.redhat.com/rhel6/rhel:6.9
RUN yum -y update; yum clean all
ADD DOCKER_VERSION .
