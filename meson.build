project(
  'gnome-control-center', 'c',
  version: '3.27.0',
  license: 'GPL2+',
  meson_version: '>= 0.43.0'
)

control_center_prefix = get_option('prefix')
control_center_bindir = join_paths(control_center_prefix, get_option('bindir'))
control_center_datadir = join_paths(control_center_prefix, get_option('datadir'))
control_center_libexecdir = join_paths(control_center_prefix, get_option('libexecdir'))
control_center_localedir = join_paths(control_center_prefix, get_option('localedir'))
control_center_mandir = join_paths(control_center_prefix, get_option('mandir'))
control_center_sysconfdir = join_paths(control_center_prefix, get_option('sysconfdir'))

control_center_pkgdatadir = join_paths(control_center_datadir, meson.project_name())
control_center_desktopdir = join_paths(control_center_datadir, 'applications')
control_center_icondir = join_paths(control_center_datadir, 'icons')

control_center_gettext = meson.project_name() + '-2.0'

host_is_linux = host_machine.system().contains('linux')
host_is_s390 = host_machine.cpu().contains('s390')

cc = meson.get_compiler('c')

config_h = configuration_data()

# defines
set_defines = [
  # package
  ['PACKAGE', meson.project_name()],
  ['PACKAGE_VERSION', meson.project_version()],
  ['VERSION', meson.project_version()],
  # i18n
  ['GETTEXT_PACKAGE', control_center_gettext]
]

foreach define: set_defines
  config_h.set_quoted(define[0], define[1])
endforeach

config_h.set('USER_DIR_MODE', 0700,
             description: 'Permissions for creating the user\'s config, cache and data directories')

# compiler flags
common_flags = ['-DHAVE_CONFIG_H']

# Only add this when optimizing is enabled (default)
optimized_src = '''
  #if __OPTIMIZE__ == 0
  #error No optimization
  #endif
'''

control_center_optimized = get_option('buildtype').contains('optimized') and cc.compiles(optimized_src)
message('whether_optimization is enabled: ' + control_center_optimized.to_string())

if control_center_optimized
  common_flags += '-Wp,-D_FORTIFY_SOURCE=2'
endif

if get_option('buildtype').contains('debug')
  test_cflags = [
    '-Wcast-align',
    '-Wmissing-field-initializers',
    '-Wmissing-declarations',
    '-Wmissing-prototypes',
    '-Wnested-externs',
    '-Wno-strict-aliasing',
    '-Wno-sign-compare'
  ]

  common_flags += cc.get_supported_arguments(test_cflags)
endif

add_project_arguments(common_flags, language: 'c')

# Check that we meet the dependencies
libgvc = subproject(
  'gvc',
  default_options: 'static=true'
)
libgvc_dep = libgvc.get_variable('libgvc_dep')

libgd = subproject(
  'libgd',
  default_options: [
    'static=true',
    'with-view-common=true',
    'with-main-view=true'
  ]
)
libgd_dep = libgd.get_variable('libgd_dep')

goa_req_version = '>= 3.25.3'
pulse_req_version = '>= 2.0'

accounts_dep = dependency('accountsservice', version: '>= 0.6.39')
clutter_gtk_dep = dependency('clutter-gtk-1.0', required: false)
colord_dep = dependency('colord', version: '>= 0.1.34')
gdk_pixbuf_dep = dependency('gdk-pixbuf-2.0', version: '>= 2.23.0')
gio_dep = dependency('gio-2.0')
glib_dep = dependency('glib-2.0', version: '>= 2.53.0')
gnome_desktop_dep = dependency('gnome-desktop-3.0', version: '>= 3.1.0')
gnome_settings_dep = dependency('gnome-settings-daemon', version: '>= 3.25.90')
goa_dep = dependency('goa-1.0', version: goa_req_version)
gsettings_desktop_dep = dependency('gsettings-desktop-schemas', version: '>= 3.27.2')
libxml_dep = dependency('libxml-2.0')
polkit_gobject_dep = dependency('polkit-gobject-1', version: '>= 0.103')
pulse_dep = dependency('libpulse', version: pulse_req_version)
pulse_mainloop_dep = dependency('libpulse-mainloop-glib', version: pulse_req_version)
upower_glib_dep = dependency('upower-glib', version: '>= 0.99.6')
x11_dep = dependency('x11')
xi_dep = dependency('xi', version: '>= 1.2')

m_dep = cc.find_library('m')

common_deps = [
  gio_dep,
  glib_dep,
  gsettings_desktop_dep,
  dependency('gio-unix-2.0'),
  dependency('gthread-2.0'),
  dependency('gtk+-3.0', version: '>= 3.22.20')
]

# Check for CUPS 1.4 or newer
cups_dep = dependency('cups', version : '>= 1.4', required: false)
assert(cups_dep.found(), 'CUPS 1.4 or newer not found')

# https://bugzilla.gnome.org/show_bug.cgi?id=696766
cups_cflags = []
if cups_dep.version().version_compare('>= 1.6')
  cups_cflags += '-D_PPD_DEPRECATED=""'
endif

# cups headers
check_headers = [
  ['HAVE_CUPS_CUPS_H', 'cups/cups.h'],
  ['HAVE_CUPS_PPD_H', 'cups/ppd.h']
]

foreach header: check_headers
  assert(cc.has_header(header[1], args: cups_cflags), 'CUPS headers not found: ' + header[1])
endforeach

# Optional dependency for the user accounts panel
enable_cheese = get_option('cheese')
if enable_cheese
  assert(clutter_gtk_dep.found(), 'cheese support requested, but clutter-gtk library is not available. Use -Dcheese=false to build without it.')

  cheese_deps = [
    clutter_gtk_dep,
    dependency('cheese'),
    dependency('cheese-gtk', version: '>= 3.5.91')
  ]
endif
config_h.set('HAVE_CHEESE', enable_cheese,
             description: 'Define to 1 to enable cheese webcam support')

# IBus support
enable_ibus = get_option('ibus')
if enable_ibus
  ibus_dep = dependency('ibus-1.0', version: '>= 1.5.2')
endif
config_h.set('HAVE_IBUS', enable_ibus,
             description: 'Defined if IBus support is enabled')

# wayland
enable_wayland = get_option('wayland')
if enable_wayland
  wayland_deps = [
    dependency('gdk-wayland-3.0'),
    dependency('gudev-1.0')
  ]
endif
config_h.set('HAVE_WAYLAND', enable_wayland,
             description: 'Define to 1 if Wayland is enabled')

if host_is_s390
  message('Bluetooth, Network and Wacom panels will not be built (no USB support on this platform)')
endif

enable_linux = host_is_linux and not host_is_s390
if enable_linux
  # gnome-bluetooth
  gnome_bluetooth_dep = dependency('gnome-bluetooth-1.0', version: '>= 3.18.2')

  # network manager
  network_manager_deps = [
    dependency('libnm', version: '>= 1.2.0'),
    dependency('libnma', version: '>= 1.2.0'),
    dependency('mm-glib', version: '>= 0.7')
  ]

  network_manager_dep = dependency('NetworkManager')
  nm_vpn_config_dir = join_paths(network_manager_dep.get_pkgconfig_variable('configdir'), 'VPN')
  nm_vpn_module_dir = network_manager_dep.get_pkgconfig_variable('plugindir')

  # Wacom
  assert(clutter_gtk_dep.found(), 'clutter-gtk library is required for wacom support, but is not available.')

  wacom_deps = [
    clutter_gtk_dep,
    dependency('clutter-1.0', version: '>= 1.11.3'),
    dependency('libwacom', version: '>= 0.7')
  ]
endif
config_h.set('BUILD_BLUETOOTH', enable_linux,
             description: 'Define to 1 to build the Bluetooth panel')
config_h.set('HAVE_BLUETOOTH', enable_linux,
             description: 'Define to 1 if bluetooth support is available')
config_h.set('BUILD_NETWORK', enable_linux,
             description: 'Define to 1 to build the Network panel')
config_h.set('HAVE_NETWORK_MANAGER', enable_linux,
             description: 'Define to 1 if NetworkManager is available')
config_h.set('BUILD_WACOM', enable_linux,
             description: 'Define to 1 to build the Wacom panel')
config_h.set('HAVE_WACOM', enable_linux,
             description: 'Define to 1 if Wacom is supportted')

# Check for info panel
gnome_session_libexecdir = get_option('gnome_session_libexecdir')
if gnome_session_libexecdir == ''
  gnome_session_libexecdir = control_center_libexecdir
endif

gnome = import('gnome')
i18n = import('i18n')
pkg = import('pkgconfig')

desktop_conf = configuration_data()
desktop_conf.set('VERSION', meson.project_version())

po_dir = join_paths(meson.source_root(), 'po')

intltool_merge = find_program('intltool-merge')
intltool_cache = join_paths(po_dir, '.intltool-merge-cache')
intltool_desktop_cmd = [intltool_merge, '-d', '-u', '-c', intltool_cache, po_dir, '@INPUT@', '@OUTPUT@']
intltool_xml_cmd = [intltool_merge, '-x', '-u', '-c', intltool_cache, po_dir, '@INPUT@', '@OUTPUT@']

top_inc = include_directories('.')
shell_inc = include_directories('shell')

update_from_gsd = find_program('update-from-gsd.sh')
update_from_gsd_in = files('update-from-gsd.in')
update_from_nma_in = files('update-from-nma.in')

subdir('data/icons')
subdir('po')
subdir('panels')
subdir('shell')
subdir('search-provider')

if get_option('documentation')
  subdir('man')
endif

configure_file(
  output: 'config.h',
  configuration: config_h
)

meson.add_install_script(
  'meson_post_install.py',
  control_center_datadir
)

output = meson.project_name() + ' was configured with the following options:\n'
output += '** gnome-bluetooth (Bluetooth panel): ' + enable_linux.to_string() + '\n'
output += '** Cheese (Users panel webcam support): ' + enable_cheese.to_string() + '\n'
output += '** IBus (Region panel IBus support): ' + enable_ibus.to_string() + '\n'
output += '** NetworkManager (Network panel): ' + enable_linux.to_string() + '\n'
output += '** wacom (Wacom tablet panel): ' + enable_linux.to_string() + '\n'
output += '** Wayland: ' + enable_wayland.to_string() + '\n'
output += '** gnome-session libexecdir: ' + gnome_session_libexecdir + '\n'
output += 'End options'
message(output)