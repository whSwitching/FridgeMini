#pragma once

#ifndef _SYSINFO_H
#define _SYSINFO_H

#include <esp32-hal-log.h>
#include <esp_ota_ops.h>
#include <esp_system.h>
#include "Utils.h"

//extern "C" int rom_phy_get_vdd33();

class SysInfo
{
public:
	SysInfo()
	{
		log_i("[SysInfo()] on core %d", xPortGetCoreID());
	}

    static void Get_Sysinfo(sys_info_t& v)
	{
		v.chipCores = ESP.getChipCores();
		v.chipID = chipID();
		v.chipModel = ESP.getChipModel();
		v.chipRevision = ESP.getChipRevision();
		v.cpuFreqMHz = ESP.getCpuFreqMHz();
		v.flashChipFreqMHz = ESP.getFlashChipSpeed() / 1000000;
		v.flashChipMode = flashChipMode();
		v.flashChipSizeKB = ESP.getFlashChipSize() / 1024.0f;
		v.freeHeapSizeKB = ESP.getFreeHeap() / 1024.0f;
		v.freePsramSizeKB = ESP.getFreePsram() / 1024.0f;
		v.freeSketchSpaceKB = ESP.getFreeSketchSpace() / 1024.0f;
		v.heapSizeKB = ESP.getHeapSize() / 1024.0f;
		v.maxAllocHeapB = ESP.getMaxAllocHeap();
		v.maxAllocPsramKB = ESP.getMaxAllocPsram() / 1024.0f;
		v.psramSizeKB = ESP.getPsramSize() / 1024.0f;
		v.sdkVersion = ESP.getSdkVersion();
		v.sketchSizeKB = ESP.getSketchSize() / 1024.0f;
		v.fsSizeKB = SPIFFS.totalBytes() / 1024.0f;
		v.usedFSSizeKB = SPIFFS.usedBytes() / 1024.0f;
		v.resetReason = esp_reset_reason();
	}

    static void Print_Sysinfo(sys_info_t* v)
	{
		log_i("---- SystemInfo ----");
		log_i("Chip ID: %d", v->chipID);
		log_i("Chip Model: %s", v->chipModel);
		log_i("CPU Cores: %d", v->chipCores);
		log_i("CPU Frequency: %d MHz", v->cpuFreqMHz);
		log_i("Flash Chip Mode: %s", v->flashChipMode);
		log_i("Flash Chip Frequency: %d MHz", v->flashChipFreqMHz);
		log_i("Flash Chip Size: %.3f KBytes", v->flashChipSizeKB);
		log_i("Sketch Size: %.3f KBytes", v->sketchSizeKB);
		log_i("Free Sketch Space: %.3f KBytes", v->freeSketchSpaceKB);
		log_i("Free Internal RAM: %.3f of %.3f KBytes", v->freeHeapSizeKB, v->heapSizeKB);
		log_i("Max Block Internal RAM: %d Bytes", v->maxAllocHeapB);
		log_i("Free PSRAM: %.3f of %.3f KBytes", v->freePsramSizeKB, v->psramSizeKB);
		log_i("Max Block PSRAM: %.3f KBytes", v->maxAllocPsramKB);
		log_i("SDK Version: %s", ESP.getSdkVersion());
		log_i("--------------------");
	}

	static void Print_PartitionTable()
	{
		log_i("---- PartitionTable ----");
		log_i("Index  Label       Type  SubType  Offset    Size");
		esp_partition_iterator_t it = esp_partition_find(ESP_PARTITION_TYPE_ANY, ESP_PARTITION_SUBTYPE_ANY, NULL);
		int idx = 0;
		for (; it != NULL; it = esp_partition_next(it))
		{
			const esp_partition_t* part = esp_partition_get(it);
			log_i("%-5d  %-10s  %-4d  %-7d  0x%-6X  0x%-6X(%d)", idx, part->label, part->type, part->subtype, part->address, part->size, part->size);
			idx++;
		}
		esp_partition_iterator_release(it);
		log_i("------------------------");
		const esp_partition_t* bp = esp_ota_get_boot_partition();
		const esp_partition_t* rp = esp_ota_get_running_partition();
		log_i("Boot partition   : %s at 0x%-6X", bp->label, bp->address);
		log_i("Running partition: %s at 0x%-6X", rp->label, rp->address);
		log_i("------------------------");
	}

private:
    static uint32_t chipID()
	{
		uint32_t chipId = 0;
		uint64_t efuseMac = ESP.getEfuseMac();
		for (int i = 0; i < 17; i = i + 8)
		{
			chipId |= ((efuseMac >> (40 - i)) & 0xff) << i;
		}
		return chipId;
	}

    static String flashChipMode()
	{
		FlashMode_t flashMode = ESP.getFlashChipMode();
		if (flashMode == FM_QIO)
			return "QIO";
		else if (flashMode == FM_QOUT)
			return "QOUT";
		else if (flashMode == FM_DIO)
			return "DIO";
		else if (flashMode == FM_DOUT)
			return "DOUT";
		else if (flashMode == FM_FAST_READ)
			return "FAST_READ";
		else if (flashMode == FM_SLOW_READ)
			return "SLOW_READ";
		else
			return "UNKNOWN";
	}
};

#endif
