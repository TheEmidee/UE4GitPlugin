Unreal Engine 4 Git Source Control Plugin
-----------------------------------------

UE4GitPlugin is a simple Git Source Control Plugin for Unreal Engine 4.x

See the website http://srombauts.github.com/UE4GitPlugin on GitHub.

### Status

Alpha version under early development: v0.1 displays status icons to show modified files, show history, can revert modifications, and add or delete files.

What cannot be done presently:
- initialize a new Git repository: you have to do it by yourself when creating a new UE4 Game Project.
- log history of a file (but work in progress)
- diff with a previous version of a file
- delete a file
- checkout a file
- checkin/commit a file
- Pull/Fetch/Push are not possible within the current Editor workflow
- Branch and Merge are not possible within the current Editor workflow (but this is on Epic Roadmap)

### Getting started

#### Install Git

Under Windows 64bits, you could either:
- install a standalone Git, usualy in "C:\Program Files (x86)\Git\bin\git.exe".
- or copy a [portable Git](https://code.google.com/p/msysgit/downloads/list?can=1&q=PortableGit)
inside "<UnrealEngine>/Engine/Binaries/ThirdParty/git/Win32".
- TODO: What about TortoiseGit?

#### Initialize your Game Project directory with Git

Use your favorite Git program to initialize and manage your whole game project directory.
For instance:

```
C:/Users/<username>/Documents/Unreal Projects/<YourGameProject>
```

#### Install this Git Plugin

There are a few differents means to use a Plugin with UE4

See also the [Plugins official Documentation](https://docs.unrealengine.com/latest/INT/Programming/Plugins/index.html)

##### Within a standard installed Unreal Engine binary release:

Clone the plugin repository under the "Plugins" directory of the Engine, beside PerforceSourceControl and SubversionSourceControl:

```
<UnrealEngineInstallation>/4.1/Engine/Plugins/Developer
```

The name of the directory is unimportant:

```bash
git clone https://github.com/SRombauts/UE4GitPlugin.git
```

##### Within an Unreal Engine source release from GitHub:

Clone the plugin repository under the "Plugins" directory of the Engine, beside PerforceSourceControl and SubversionSourceControl:

```
<UnrealEngineClone>/Engine/Plugins/Developer
```

taking care of using the name **GitSourceControl** as destination directory (same name as the "GitSourceControl.uplugin" file):

```bash
git clone https://github.com/SRombauts/UE4GitPlugin.git GitSourceControl
```

##### Within your Game Project only

Alternatively, you could choose to install the plugin into a subfolder or your Game Project "Plugins" directory:

```
<YourGameProject>/Plugins
```

#### Enable Git Plugin within the Editor

Launch the UE4 Editor, then open:

```
Windows->Plugins, enable Editor->Source Control->Git
```

click Enable and restart the Editor.

#### Activate Git Source Control for your Game Project

Load your Game Project, then open:

```
File->Connect To Source Control... -> Git: Accept Settings
```

See also the [Source Control official Documentation](https://docs.unrealengine.com/latest/INT/Engine/UI/SourceControl/index.html)

### License

Copyright (c) 2014 Sébastien Rombauts (sebastien.rombauts@gmail.com)

Distributed under the MIT License (MIT) (See accompanying file LICENSE.txt
or copy at http://opensource.org/licenses/MIT)

## How to contribute
### GitHub website
The most efficient way to help and contribute to this wrapper project is to
use the tools provided by GitHub:
- please fill bug reports and feature requests here: https://github.com/SRombauts/UE4GitPlugin/issues
- fork the repository, make some small changes and submit them with independant pull-request

### Contact
You can also email me directly, I will answer any questions and requests.

### Coding Style Guidelines
The source code follow the UnreaEngine official [Coding Standard](https://docs.unrealengine.com/latest/INT/Programming/Development/CodingStandard/index.html) :
- CamelCase naming convention, with a prefix letter to differentiate classes ('F'), interfaces ('I'), templates ('T')
- files (.cpp/.h) are named like the class they contains
- Doxygen comments, documentation is located with declaration, on headers
- Use portable common features of C++11 like nullptr, auto, range based for, OVERRIDE macro
- Braces on their own line
- Tabs to indent code, with a width of 4 characters

## See also - Some other useful UE4 plugins:

- [ue4-hg-plugin](https://github.com/enlight/ue4-hg-plugin)

