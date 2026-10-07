## Optional for the flatpak packager

In order to generate the project flatpak package install the `flatpak-builder` command:

```
sudo apt install -y flatpak flatpak-builder
```

You'll also need the GNOME runtime and SDK the package is built with, the ones of the GTK 4 the [darknetxx-gui binary](/doc/sections/en_US/5-36-the-darknetxx-gui-binary.md) shows it's window with:

```
flatpak remote-add --user --if-not-exists flathub https://dl.flathub.org/repo/flathub.flatpakrepo
flatpak install --user flathub org.gnome.Platform//51 org.gnome.Sdk//51
```

Look for the details at the [Enabling the flatpak package generation support](/doc/sections/en_US/5-11-enabling-the-flatpak-package-generation-support.md).
