# -*- mode: ruby -*-
# vi: set ft=ruby :

Vagrant.configure(2) do |config|
  config.vm.box = "fathens/centos-node43_opencv24"

  config.vm.provider "virtualbox" do |vb|
    vb.memory = 1024
  end

  if Vagrant.has_plugin?("vagrant-timezone")
    config.timezone.value = :host
  end

  config.vm.provision "shell", inline: <<-SHELL
    su - vagrant -c 'cd /vagrant && npm uninstall opencv'
    su - vagrant -c 'cd /vagrant && PKG_CONFIG_PATH=/usr/local/opencv/lib/pkgconfig/ npm install opencv'
  SHELL
end
