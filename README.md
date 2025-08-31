# PingTracer - Professional Network Diagnostic Tool

![PingTracer Logo](https://via.placeholder.com/200x80/4299e1/ffffff?text=PingTracer)

**Developer:** Harvey  
**Website:** [www.iqterabharvey.me](https://www.iqterabharvey.me)  
**Version:** 1.0.0  

## Overview

PingTracer is a modern, cross-platform network diagnostic application that provides real-time traceroute functionality similar to WinMTR, but with enhanced features and a professional user interface. Built with Qt6 and C++17, it offers comprehensive network path analysis with beautiful themes and export capabilities.

## Features

### 🔍 **Network Diagnostics**
- **Real-time Traceroute**: Continuous path tracing to any hostname or IP address
- **Per-hop Statistics**: Detailed latency statistics for each network hop
- **Packet Loss Monitoring**: Visual indication of packet loss with color coding
- **Hostname Resolution**: Automatic DNS resolution for each hop
- **Configurable Parameters**: Adjustable ping interval and timeout settings

### 🎨 **Professional Interface**
- **Modern UI Design**: Clean, professional interface with custom styling
- **Dark Mode Support**: Toggle between light and dark themes
- **Color-coded Results**: Visual indicators for network performance
- **Responsive Layout**: Resizable panels and adaptive interface
- **Real-time Updates**: Live updating of statistics and results

### 📊 **Data Management**
- **Export Functionality**: Export results to TXT and CSV formats
- **Copy to Clipboard**: Quick copy of formatted results
- **Statistics Panel**: Detailed network statistics and logging
- **Result History**: Track and analyze network performance over time

### 🔧 **Cross-platform Support**
- **Windows Compatibility**: Full Windows support with native look
- **Linux Support**: Native Linux integration
- **Qt6 Framework**: Modern, maintained framework
- **Professional Build System**: CMake-based build configuration

## Screenshots

### Professional Theme (Light)
![Professional Theme](https://via.placeholder.com/800x600/f5f5f5/333333?text=Professional+Theme+Preview)

### Dark Theme
![Dark Theme](https://via.placeholder.com/800x600/1a202c/e2e8f0?text=Dark+Theme+Preview)

## Installation

### Prerequisites
- **Qt6** (6.2 or later)
- **CMake** (3.16 or later)
- **C++17 compatible compiler**
- **Network permissions** for raw socket access

### Building from Source

#### Windows
```cmd
# Clone the repository
git clone https://github.com/Iqterab9/PingTracer.git
cd PingTracer

# Create build directory
mkdir build
cd build

# Configure with CMake
cmake .. -G "Visual Studio 17 2022" -A x64

# Build
cmake --build . --config Release

# Run
Release\PingTracer.exe
```

#### Linux
```bash
# Clone the repository
git clone https://github.com/Iqterab9/PingTracer.git
cd PingTracer

# Install dependencies (Ubuntu/Debian)
sudo apt-get update
sudo apt-get install qt6-base-dev qt6-tools-dev cmake build-essential

# Create build directory
mkdir build
cd build

# Configure and build
cmake ..
make -j$(nproc)

# Run
./PingTracer
```

### Package Installation

#### Windows
1. Download the latest release from [Releases](https://github.com/Iqterab9/PingTracer/releases)
2. Run the installer: `PingTracer-Setup-v1.0.0.exe`
3. Follow the installation wizard
4. Launch from Start Menu or Desktop shortcut

#### Linux
```bash
# Download and install .deb package (Ubuntu/Debian)
wget https://github.com/Iqterab9/PingTracer/releases/download/v1.0.0/pingtracer_1.0.0_amd64.deb
sudo dpkg -i pingtracer_1.0.0_amd64.deb

# Or download and install .rpm package (RHEL/CentOS/Fedora)
wget https://github.com/Iqterab9/PingTracer/releases/download/v1.0.0/pingtracer-1.0.0.x86_64.rpm
sudo rpm -i pingtracer-1.0.0.x86_64.rpm
```

## Usage

### Quick Start
1. **Launch PingTracer**
2. **Enter Target**: Type a hostname or IP address (e.g., `google.com`, `8.8.8.8`)
3. **Configure Settings**: Adjust interval and timeout if needed
4. **Start Tracing**: Click the "Start" button
5. **Monitor Results**: Watch real-time statistics in the results table
6. **Export Data**: Use "Export" or "Copy to Clipboard" to save results

### Interface Guide

#### Input Panel
- **Host/IP Field**: Enter the target hostname or IP address
- **Interval**: Time between ping packets (100-10000ms)
- **Timeout**: Maximum wait time for responses (500-30000ms)
- **Control Buttons**: Start, Stop, and Reset operations

#### Results Table
- **Hop**: Router number in the path to destination
- **Hostname**: DNS name of the router (if available)
- **IP Address**: Router's IP address
- **Loss %**: Percentage of packets lost at this hop
- **Sent**: Number of packets sent to this hop
- **Best/Avg/Worst**: Latency statistics in milliseconds

#### Color Coding
- 🟢 **Green**: Low packet loss (≤10%)
- 🟡 **Yellow**: Medium packet loss (10-50%)
- 🔴 **Red**: High packet loss (>50%)

#### Statistics Panel
- Real-time network statistics
- Detailed hop information
- Event logging
- Performance metrics

### Keyboard Shortcuts
- **Ctrl+N**: Start tracing
- **Ctrl+T**: Stop tracing
- **Ctrl+R**: Reset results
- **Ctrl+S**: Export results
- **F1**: Show help
- **Ctrl+Q**: Exit application

## Technical Details

### Architecture
```
PingTracer/
├── src/                    # Source code
│   ├── main.cpp           # Application entry point
│   ├── mainwindow.*       # Main UI window
│   ├── pingtracer.*       # Core tracing logic
│   ├── networktester.*    # Network testing component
│   ├── thememanager.*     # Theme management
│   └── exportmanager.*    # Export functionality
├── ui/                    # UI definition files
├── resources/             # Icons, styles, themes
├── docs/                  # Documentation
└── scripts/               # Build and packaging scripts
```

### Dependencies
- **Qt6 Core**: Core Qt functionality
- **Qt6 Widgets**: GUI components
- **Qt6 Network**: Network operations
- **Standard C++ Libraries**: STL containers and algorithms

### Network Implementation
- **Cross-platform Networking**: Uses Qt's network abstraction
- **ICMP Simulation**: UDP-based ping simulation for demo
- **Thread-safe Operations**: Mutex-protected data structures
- **Asynchronous Operations**: Non-blocking network operations

## Configuration

### Settings File
PingTracer stores settings in platform-specific locations:
- **Windows**: `%APPDATA%/Harvey Tech/PingTracer/settings.ini`
- **Linux**: `~/.config/Harvey Tech/PingTracer/settings.ini`

### Configurable Options
```ini
[General]
theme=Professional
interval=1000
timeout=5000
maxHops=30
autoResolveHostnames=true

[UI]
darkMode=false
windowGeometry=1200x800
splitterSizes=800,400
```

## Troubleshooting

### Common Issues

#### Permission Denied (Linux)
```bash
# Run with elevated privileges for raw socket access
sudo ./PingTracer

# Or set capabilities (preferred)
sudo setcap cap_net_raw+ep ./PingTracer
```

#### Missing Qt6 Libraries
```bash
# Ubuntu/Debian
sudo apt-get install qt6-base-dev

# RHEL/CentOS/Fedora
sudo dnf install qt6-qtbase-devel

# Arch Linux
sudo pacman -S qt6-base
```

#### Build Errors
1. **Check Qt6 Installation**: Ensure Qt6 is properly installed
2. **Update CMake**: Use CMake 3.16 or later
3. **Compiler Support**: Ensure C++17 support
4. **Clean Build**: Remove build directory and rebuild

### Performance Tips
- **Reduce Interval**: Lower intervals provide more data but use more bandwidth
- **Adjust Timeout**: Shorter timeouts for faster results, longer for slow networks
- **Limit Hops**: Reduce max hops for faster startup on local networks

## Contributing

We welcome contributions to PingTracer! Please see our [Contributing Guide](CONTRIBUTING.md) for details.

### Development Setup
1. Fork the repository
2. Create a feature branch: `git checkout -b feature/amazing-feature`
3. Make your changes
4. Add tests if applicable
5. Commit your changes: `git commit -m 'Add amazing feature'`
6. Push to the branch: `git push origin feature/amazing-feature`
7. Submit a Pull Request

### Code Style
- Follow Qt coding conventions
- Use meaningful variable names
- Add comments for complex logic
- Maintain consistent formatting

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Support

### Getting Help
- **Documentation**: Check this README and inline help
- **Issues**: [GitHub Issues](https://github.com/Iqterab9/PingTracer/issues)
- **Discussions**: [GitHub Discussions](https://github.com/Iqterab9/PingTracer/discussions)
- **Email**: Contact Harvey at [harvey@iqterabharvey.me](mailto:harvey@iqterabharvey.me)

### Reporting Bugs
When reporting bugs, please include:
- Operating system and version
- Qt version
- Steps to reproduce
- Expected vs actual behavior
- Screenshots if applicable

## Roadmap

### Version 1.1.0
- [ ] IPv6 support
- [ ] Advanced filtering options
- [ ] Network topology visualization
- [ ] Performance benchmarking

### Version 1.2.0
- [ ] Plugin system
- [ ] Custom alert thresholds
- [ ] Historical data analysis
- [ ] REST API for automation

### Version 2.0.0
- [ ] Real-time graphing
- [ ] Network mapping
- [ ] Multi-target monitoring
- [ ] Cloud integration

## Acknowledgments

- **Qt Project**: For the excellent Qt framework
- **WinMTR**: For inspiration and reference
- **Community**: For feedback and contributions
- **Testing**: Beta testers and early adopters

## About the Developer

**Harvey** is a passionate software developer specializing in network tools and system utilities. With expertise in C++, Qt, and cross-platform development, Harvey creates professional-grade applications that solve real-world problems.

**Website**: [www.iqterabharvey.me](https://www.iqterabharvey.me)  
**GitHub**: [@Iqterab9](https://github.com/Iqterab9)  
**Contact**: [harvey@iqterabharvey.me](mailto:harvey@iqterabharvey.me)

---

*PingTracer - Professional Network Diagnostics Made Simple*

**Copyright © 2024 Harvey Tech. All rights reserved.**
