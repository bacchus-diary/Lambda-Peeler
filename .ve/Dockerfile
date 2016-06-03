FROM fathens/docker-lambda-peeler

RUN set -x \
 && yum update -y \
 && yum install -y openssh-server sudo \
 && /usr/sbin/sshd-keygen

RUN set -x \
  && adduser vagrant \
  && (echo vagrant; echo vagrant) | passwd vagrant \
  && mkdir -vp ~vagrant/.ssh \
  && curl -L https://raw.github.com/mitchellh/vagrant/master/keys/vagrant.pub > ~vagrant/.ssh/authorized_keys \
  && chown -R vagrant:vagrant ~vagrant/.ssh \
  && chmod 700 ~vagrant/.ssh \
  && chmod 600 ~vagrant/.ssh/authorized_keys \
  && ls -la ~vagrant/.ssh

RUN set -x \
  && echo 'vagrant ALL=(ALL) NOPASSWD: ALL' > /etc/sudoers.d/vagrant \
  && chmod 0440 /etc/sudoers.d/vagrant \
  && cat /etc/sudoers | sed 's/^\(Defaults\s\+requiretty\)/#\1/' > /etc/sudoers.sed \
  && mv -vf /etc/sudoers.sed /etc/sudoers

CMD ["/usr/sbin/sshd", "-D"]
