################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
RobotLibrary/LCD/src/grlib/%.obj: ../RobotLibrary/LCD/src/grlib/%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"C:/ti/ccs1031/ccs/tools/compiler/ti-cgt-arm_20.2.4.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me --include_path="C:/ti/ccs1031/ccs/ccs_base/arm/include" --include_path="C:/Users/Austin/OneDrive/Classes/Project Tidbits/Room Mapping Robot/Room-Mapping-Robot/RobotLibrary/OPT3101" --include_path="C:/Users/Austin/OneDrive/Classes/Project Tidbits/Room Mapping Robot/Room-Mapping-Robot/RobotLibrary/LCD/src/LcdDriver" --include_path="C:/Users/Austin/OneDrive/Classes/Project Tidbits/Room Mapping Robot/Room-Mapping-Robot/BoardSupportPackage/DriverLib" --include_path="C:/Users/Austin/OneDrive/Classes/Project Tidbits/Room Mapping Robot/Room-Mapping-Robot/BoardSupportPackage/inc" --include_path="C:/Users/Austin/OneDrive/Classes/Project Tidbits/Room Mapping Robot/Room-Mapping-Robot/BoardSupportPackage/src" --include_path="C:/Users/Austin/OneDrive/Classes/Project Tidbits/Room Mapping Robot/Room-Mapping-Robot/RobotLibrary/LCD/src/fonts" --include_path="C:/Users/Austin/OneDrive/Classes/Project Tidbits/Room Mapping Robot/Room-Mapping-Robot/RobotLibrary/LCD/src/grlib" --include_path="C:/Users/Austin/OneDrive/Classes/Project Tidbits/Room Mapping Robot/Room-Mapping-Robot/RobotLibrary/Motors" --include_path="C:/ti/ccs1031/ccs/ccs_base/arm/include/CMSIS" --include_path="C:/Users/Austin/OneDrive/Classes/Project Tidbits/Room Mapping Robot/Room-Mapping-Robot" --include_path="C:/ti/ccs1031/ccs/tools/compiler/ti-cgt-arm_20.2.4.LTS/include" --advice:power="all" --define=__MSP432P401R__ --define=ccs -g --c11 --gcc --diag_warning=225 --diag_wrap=off --display_error_number --abi=eabi --preproc_with_compile --preproc_dependency="RobotLibrary/LCD/src/grlib/$(basename $(<F)).d_raw" --obj_directory="RobotLibrary/LCD/src/grlib" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


