--[[----------------------------------------------------------------------------
The content of this file includes portions of the AUDIOKINETIC Wwise Technology
released in source code form as part of the SDK installer package.

Commercial License Usage

Licensees holding valid commercial licenses to the AUDIOKINETIC Wwise Technology
may use this file in accordance with the end user license agreement provided
with the software or, alternatively, in accordance with the terms contained in a
written agreement between you and Audiokinetic Inc.

  Copyright (c) 2020 Audiokinetic Inc.
------------------------------------------------------------------------------]]


if not AK then AK = {} end
if not AK.Platforms then AK.Platforms = {} end

local UWPPlatforms = function()
	if _ACTION == "vs2015" then
		platforms({ "ARM", "Win32", "x64" })
	else
		platforms({ "ARM", "ARM64", "Win32", "x64" })
	end
end

AK.Platforms.UWP =
{
	name = "UWP",
	directories = {
		src = {
			__default__ = "UWP",
			AkJobMgr = "Win32",
			AkMusicEngine = "Win32",
			GameSimulator = "Win32",
			AkSink = "Win32",
			AkVorbisDecoder = "Win32",
		},
		include = {
			AkMemoryMgr = "Win32"
		},
		simd = "SIMD",
		project = "UWP",
		lualib = "Win32",
		lowlevelio = "Win32",
		luasln = "GameSimulator/source/",
	},
	kinds = {
		GameSimulator = "ConsoleApp",
		IntegrationDemo = "WindowedApp"
	},
	suffix = "UWP",

	configurations =
	{
		"Debug",
		"Profile",
		"Profile_EnableAsserts",
		"Release",
	},
	platforms = UWPPlatforms,
	avx2archs = { "x64" },
	avxarchs = { "x64" },
	features = { "Motion", "iZotope", "IntegrationDemo" },
	validActions = { "vs2015", "vs2017", "vs2019" },
	
	AdditionalSoundEngineProjects = function()
		return {}
	end,
	AddActionSuffixToDllProjects = true,

	-- API
	---------------------------------
	ImportAdditionalWwiseSDKProjects = function()
	end,

	-- Project factory. Creates "StaticLib" target by default. Static libs (only) are added to the global list of targets.
	-- Other target types supported by premake are "WindowedApp", "ConsoleApp" and "SharedLib".
	-- Upon returning from this method, the current scope is the newly created project.
	CreateProject = function(in_fileName, in_targetName, in_projectLocation, in_suffix, pathPCH, in_targetType)
		verbosef("        Creating project: %s", in_targetName)

		-- Make sure that directory exist
		os.mkdir(AkMakeAbsolute(in_projectLocation))

		-- Create project
		local prj = project(in_targetName)
			UWPPlatforms()

			location(AkRelativeToCwd(in_projectLocation))
			targetname(in_targetName)
			if in_targetType == nil or in_targetType == "StaticLib" then
				kind("StaticLib")
				-- Add to global table
				_AK_TARGETS[in_targetName] = in_fileName
				--	Ignore linker warnings :
				-- "archiving object file compiled with /ZW into a static library..."
				linkoptions { "/IGNORE:4264" }
			else
				kind(in_targetType)
				libdirs{"$(OutDir)../lib",}
			end
			language("C++")
			uuid(GenerateUuid(in_fileName))
			filename(in_fileName)
			symbols "on"
			symbolspath "$(OutDir)$(TargetName).pdb"
			flags { "OmitUserFiles", "ForceFiltersFiles" } -- We never want .user files, we always want .filters files.

			--	Ignore linker warnings :
			-- "This object file does not define any previously undefined public symbols"
			linkoptions { "/IGNORE:4221" }

			-- Common flags.
			exceptionhandling "On"

			characterset "Unicode"
			buildoptions { "/utf-8" }
			-- Precompiled headers.
			if pathPCH ~= nil then
				files
				{
					AkRelativeToCwd(pathPCH) .. "stdafx.cpp",
					AkRelativeToCwd(pathPCH) .. "stdafx.h",
				}
				--pchheader ( AkRelativeToCwd(pathPCH) .. "stdafx.h" )
				pchheader "stdafx.h"
				pchsource ( AkRelativeToCwd(pathPCH) .. "stdafx.cpp" )
			end

			-- Standard configuration settings.
			filter "*Debug*"
				defines ("_DEBUG")

			filter "Profile*"
				defines ("NDEBUG")
				optimize ("Speed")

			filter "*Release*"
				defines ("NDEBUG")
				optimize ("Speed")

			if not _AK_BUILD_AUTHORING then
				filter "Release*"
					defines ("AK_OPTIMIZED")
			end

			-- Add architecture specific defines.
			filter "platforms:Win32"
				architecture "x86"
				defines "WIN32"
				vectorextensions "SSE"
			filter "platforms:x64"
				architecture "x86_64"
				defines "WIN64"
			filter "platforms:ARM"
				architecture "arm"

			filter "action:vs2015"
				systemversion "10.0.14393.0"

			filter "action:vs2017 or vs2019"
				systemversion "10.0.17763.0"

			-- Disable small exception handling optimizations introduced in VS 2019 16.3
			-- since they add a dependency on the combined 2015-2019 vc++ redists (for vc140runtime_1.dll)
			filter "toolset:msc-v142"
				buildoptions { "/d2FH4-" }
				linkoptions { "/d2:-FH4-" }

			filter {}

			-- Style sheets.
			local ssext = ".props"

			filter "*Debug*"
				vs_propsheet(AkRelativeToCwd(_AK_ROOT_DIR) .. "PropertySheets/UWP/Debug" .. in_suffix .. ssext)
			filter "*Profile* or *Release*"
				vs_propsheet(AkRelativeToCwd(_AK_ROOT_DIR) .. "PropertySheets/UWP/NDebug" .. in_suffix .. ssext)
			DisablePropSheetElements()
			filter {}
				removeelements {
					"TargetExt"
				}

			filter "*EnableAsserts"
				defines( "AK_ENABLE_ASSERTS" )

			-- Set the scope back to current project
			project(in_targetName)

		ApplyPlatformExceptions(prj.name, prj)

		return prj
	end,

	-- Plugin factory.
	-- Upon returning from this method, the current scope is the newly created plugin project.
	CreatePlugin = function(in_fileName, in_targetName, in_projectLocation, in_suffix, pathPCH, in_targetType)
		local prj = AK.Platforms.UWP.CreateProject(in_fileName, in_targetName, in_projectLocation, in_suffix, pathPCH, in_targetType)
		return prj
	end,

	Exceptions = {
		AkMemoryMgr = function(prj)
			flags("NoWinRT")
		end,
		AkOpusDecoder = function(prj)
			local prjLoc = AkRelativeToCwd(prj.location)
			includedirs { 	
				prjLoc .. "/../../../AkAudiolib/Win32",
			}
			flags("NoWinRT")
			filter "platforms:ARM"
				defines("WIN32")
			filter {}
		end,
		AkSink = function(prj)
			local prjLoc = AkRelativeToCwd(prj.location)
			includedirs {
				prjLoc .. "/../../../../AkAudiolib/Win32"
			}
		end,
		AkSoundEngine = function(prj)
			local prjLoc = AkRelativeToCwd(prj.location)
			includedirs {
				prjLoc .. "/../Win32"
			}
			filter "action:vs2015"
				includedirs {
					prjLoc .. "/../Win32/vc140"
				}
			filter{}
			files {
				prjLoc .. "/../Win32/*.cpp",
				prjLoc .. "/../*.h",
			}
		end,
		AkSpatialAudio = function(prj)
			local prjLoc = AkRelativeToCwd(prj.location)
			includedirs { 
				prjLoc .. "/../Win32",
				prjLoc .. "/../../SoundEngine/AkAudiolib/Win32",
			}
			files {
				prjLoc .. "/../Win32/*.cpp",
				prjLoc .. "/../Win32/*.h",
			}
		end,
		AkStreamMgr = function(prj)
			local prjLoc = AkRelativeToCwd(prj.location)
			includedirs { 
				prjLoc .. "/../Win32",
				prjLoc .. "/../../SoundEngine/AkAudiolib/Win32",
			}
			files {
				prjLoc .. "/../Win32/*.cpp",
				prjLoc .. "/../Win32/*.h",
			}
		end,
		AkVorbisDecoder = function(prj)
			local prjLoc = AkRelativeToCwd(prj.location)
			includedirs { 
				prjLoc .. "/../../../AkAudiolib/UWP" 
			}
		end,
		AuroHeadphoneFX = function(prj)
			flags("NoWinRT")
		end,
		MasteringSuiteFX = function(prj)
			flags("NoWinRT")
		end,
		CommunicationCentral = function(prj)
			local prjLoc = AkRelativeToCwd(prj.location)
			includedirs { 
				prjLoc .. "/../../../AkAudiolib/Win32",
			}
		end,
		PluginFactory = function(prj)
			local prjLoc = AkRelativeToCwd(prj.location)
			includedirs {
				prjLoc .. "/../../../../AkAudiolib/Win32",
			}
			runtime "Release"
		end,
		GameSimulator = function(prj)
			local prjLoc = AkRelativeToCwd(prj.location)

			defines {
				"_CRT_SECURE_NO_WARNINGS",
			}

			flags {
				"WinRT",
			}

			includedirs {
				prjLoc .. "/../../../../SDK/samples/IntegrationDemo/XboxOne/SampleFramework"
			}
			filter "Debug*"
				libdirs {prjLoc .. "/../../../../SDK/UWP/Debug/lib"}
			filter "Profile*"
				libdirs {prjLoc .. "/../../../../SDK/UWP/Profile/lib"}
			filter "Release*"
				libdirs {prjLoc .. "/../../../../SDK/UWP/Release/lib"}
			filter {}
		end,
		LuaLib = function(prj)
			flags("NoWinRT")
		end,
		ToLuaLib = function(prj)
			flags("NoWinRT")
		end,
		IntegrationDemoMotion = function(prj)
			AK.Platforms.UWP.Exceptions.IntegrationDemo(prj, "Motion")
		end,
		IntegrationDemo = function(prj, in_kind)
			local prjLoc = AkRelativeToCwd(prj.location)
			if in_kind == nil then in_kind = "" end

			files {
				prjLoc .. "//DX/*.h",
				prjLoc .. "//DX/*.cpp",
				prjLoc .. "/IntegrationDemo" .. in_kind .. ".appxmanifest"
			}
			vs_propsheet(prjLoc .. "/IntegrationDemo" .. in_kind .. ".props")
			libdirs {
				"$(OutDir)../../lib"
			}
			
			filter "Debug*"
				links "CommunicationCentral"
			filter "Profile*"
				links "CommunicationCentral"
			filter {}
		end,
	},

	Exclusions = {
		AkSoundEngine = function(prjLoc)
			excludes { 
				prjLoc .. "../Win32/stdafx.cpp",
				prjLoc .. "../Win32/stdafx.h",
				prjLoc .. "../Win32/AkStackTrace.cpp",
			}
		end,
		AkMotionSink = function(prjLoc)
			excludes {
				prjLoc .. "/Win32/DirectInputSink.*",
			}
		end,
		AkSpatialAudio = function(prjLoc)
			excludes { 
				prjLoc .. "/../Win32/stdafx.cpp",
				prjLoc .. "/../Win32/stdafx.h",
			}
		end,
		AkStreamMgr = function(prjLoc)
			excludes { 
				prjLoc .. "/../Win32/stdafx.cpp",
				prjLoc .. "/../Win32/stdafx.h",
			}
		end,
		GameSimulator = function(prjLoc)
			excludes {
				prjLoc .. "/../../src/libraries/Common/UniversalScrollBuffer.*",
				prjLoc .. "/../../src/libraries/Common/NetworkLowLevelIODeferred.*",
				prjLoc .. "/../../src/libraries/Common/RAMLowLevelIOHook.*",
				prjLoc .. "/../../../../SDK/samples/SoundEngine/Win32/AkDefaultIOHookDeferred.cpp"
			}
		end,
		IntegrationDemo = function(projectLocation)
			excludes {
				projectLocation .. "../Common/stdafx.cpp"
			}
		end
	}
}
return AK.Platforms.UWP
