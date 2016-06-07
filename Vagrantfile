# -*- mode: ruby -*-
# vi: set ft=ruby :

Vagrant.configure(2) do |config|
  config.vm.provider "docker" do |d|
    d.build_dir = ".ve"
    d.vagrant_vagrantfile = ".ve/Vagrantfile"
    d.force_host_vm = true
    d.has_ssh = true
    d.pull = true
    d.create_args = ["--privileged"]
  end

  if Vagrant.has_plugin?("vagrant-timezone")
    config.timezone.value = :host
  end

  config.vm.provision "shell", inline: <<-SHELL
    yum install -y ntpdate && ntpdate -bv ntp.nict.jp
    su - vagrant -c 'cd /vagrant && npm install'
  SHELL
end
