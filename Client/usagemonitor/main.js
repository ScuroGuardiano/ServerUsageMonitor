	function start()
	{
		setInterval(update, 1000);
	}
	$(start);	
	
	function decodeData(data)
	{
		if(data && (data != "Wrong data" || data != "Connect Error"))
		{
			return JSON.parse(data);
		}
		else
		{
			return false;
		}
	}
////////////////////////////////////////////////////////////////////////////////	
	function update()
	{
		$.ajax({
		type     : "GET",
		url      : "http://scuroguardiano.me/GetUsageInfo.php",
		data     : { },
		success: applyUpdate,
		complete: function() {
		
		},
		error: function(jqXHR, errorText, errorThrown) {
			console.log(errorText);
    }
	});
	}
////////////////////////////////////////////////////////////////////////////////
	function applyUpdate(data)
	{
		data = decodeData(data);
		console.log(data);
		if(data)
		{
			var totalRAM = data['Total RAM'];
			var RAMUsage = data['RAM Usage'];
			var totalVirtualMem = data['Total Virtual Memory'];
			var VirtualMemUsage = data['Virtual Memory Usage'];
			var CPUUsage = data['CPU Usage'];
			
			updateRAM(totalRAM, RAMUsage);
			updateVirtualMemory(totalVirtualMem, VirtualMemUsage);
			updateCPU(CPUUsage);
		}
		else
		{
			$('#status').html("Error: decode function returned false :(");
		}
	}
////////////////////////////////////////////////////////////////////////////////
	function updateRAM(totalRAM, RAMUsage)
	{
		totalRAM = Number(totalRAM);
		RAMUsage = Number(RAMUsage);
		if(totalRAM == NaN || RAMUsage == NaN)
		{
			$('#RAMBar').css({width: "100%", color: "red"});
			$('#RAMLabel').html("Error: No RAM Data");
		}
		else
		{
			RAMUsagePercent = Math.round((RAMUsage / totalRAM) * 100);
			totalRAM = Math.round(totalRAM / 1048576);
			RAMUsage = Math.round(RAMUsage / 1048576);
			$('#RAMBar').css({width: RAMUsagePercent.toString() + '%'});
			$('#RAMLabel').html(RAMUsage.toString() + "MB / " + totalRAM.toString() + "MB (" + RAMUsagePercent.toString() + "%)");
		}
	}
////////////////////////////////////////////////////////////////////////////////
	function updateVirtualMemory(totalVM, VMUsage)
	{
		totalVM = Number(totalVM);
		VMUsage = Number(VMUsage);
		if(totalVM == NaN || VMUsage == NaN)
		{
			$('#VirtualMemBar').css({width: "100%", color: "red"});
			$('#VirtualMemLabel').html("Error: No Virtual Memory Data");
		}
		else
		{
			VMUsagePercent = Math.round((VMUsage / totalVM) * 100);
			totalVM = Math.round(totalVM / 1048576);
			VMUsage = Math.round(VMUsage / 1048576);
			$('#VirtualMemBar').css({width: VMUsagePercent.toString()+ '%'});
			$('#VirtualMemLabel').html(VMUsage.toString() + "MB / " + totalVM.toString() + "MB (" + VMUsagePercent.toString() + "%)");
		}
	}
////////////////////////////////////////////////////////////////////////////////
	function updateCPU(CPUUsage)
	{
		CPUUsage = Number(CPUUsage);
		if(CPUUsage == NaN)
		{
			$('CPUBar').css({width: "100%", color: "red"});
			$('CPULabel').html("Error: No CPU Data");
		}
		else
		{
			CPUUsage = Math.round(CPUUsage);
			$('#CPUBar').css({width: CPUUsage.toString() + '%'});
			$('#CPULabel').html(CPUUsage.toString() + '%');
		}
	}