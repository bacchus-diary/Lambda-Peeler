# -*- mode: ruby -*-
# vi: set ft=ruby :

Vagrant.configure(2) do |config|
  config.vm.provider "docker" do |d|
    d.build_dir = ".ve"
    d.vagrant_vagrantfile = ".ve/Vagrantfile"
    d.force_host_vm = true
    d.has_ssh = true
  end

  if Vagrant.has_plugin?("vagrant-timezone")
    config.timezone.value = :host
  end

  config.vm.provision "shell", inline: <<-SHELL
    su - vagrant -c 'cd /vagrant && npm uninstall opencv'
    su - vagrant -c 'cd /vagrant && PKG_CONFIG_PATH=/usr/local/opencv/lib/pkgconfig/ npm install opencv'
  SHELL
end
