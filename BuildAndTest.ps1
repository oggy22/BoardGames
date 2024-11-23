# Define the full path to the MSBuild executable for Visual Studio 2022
$msbuildPath = "C:\Program Files\Microsoft Visual Studio\2022\Enterprise\MSBuild\Current\Bin\MSBuild.exe"

# Define the path to your solution or project file
$projectPath = "BoardGames.sln"

# Define the configuration you want to build (e.g., Debug or Release)
Foreach ($configuration in "Release", "Debug") {
	# Run MSBuild with the specified parameters
	& $msbuildPath $projectPath /p:Configuration=$configuration /p:Platform="x64"

	# Run tests
	$testExecutable = ".\x64\$($configuration)\EngineTests.exe"
	& $testExecutable
}