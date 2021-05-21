--------------------------------
--      Motor.lua by MAP      --
--------------------------------

-- Note: Only works for Platinum (en and fr) for now

--------------------------------

user_seed = 0xD8C0CB15
user_pointer = 0 --0x0227116C
user_tid = 34952
user_sid = 16938
user_lang = "it" --refer to the language table below for now, I'll implement an automatic language detection at some point

--------------------------------

tabl = {}

LANGUAGES = {"jp", "en", "fr", "it", "ge", "_6", "sp", "ko", "_9"}
--detect version and language

--RNG_LOC = {0, 0x021BFB14, 0x021BFCF4, 0x021BFC74, 0, 0, 0, 0}
--BPT_LOC = {0, 0x02101D2C, 0x02101F0C, 0x02101E8C, 0, 0, 0, 0}
WILD_OFF = 0x58E3C
OG_WILD_OFF = 0x3740C --{dp, plat}
FIRST_PARTY_OFF = 0xD094
TID_OFF = 0xD06C

CHAR_W = 14
CHAR_H = 10

MULTSPA = {
 0x41C64E6D, 0xC2A29A69, 0xEE067F11, 0xCFDDDF21, 0x5F748241, 0x8B2E1481, 0x76006901, 0x1711D201,
 0xBE67A401, 0xDDDF4801, 0x3FFE9001, 0x90FD2001, 0x65FA4001, 0xDBF48001, 0xF7E90001, 0xEFD20001,
 0xDFA40001, 0xBF480001, 0x7E900001, 0xFD200001, 0xFA400001, 0xF4800001, 0xE9000001, 0xD2000001,
 0xA4000001, 0x48000001, 0x90000001, 0x20000001, 0x40000001, 0x80000001, 0x00000001, 0x00000001}

MULTSPB = {
 0x00006073, 0xE97E7B6A, 0x31B0DDE4, 0x67DBB608, 0xCBA72510, 0x1D29AE20, 0xBA84EC40, 0x79F01880,
 0x08793100, 0x6B566200, 0x803CC400, 0xA6B98800, 0xE6731000, 0x30E62000, 0xF1CC4000, 0x23988000,
 0x47310000, 0x8E620000, 0x1CC40000, 0x39880000, 0x73100000, 0xE6200000, 0xCC400000, 0x98800000,
 0x31000000, 0x62000000, 0xC4000000, 0x88000000, 0x10000000, 0x20000000, 0x40000000, 0x80000000}

BLOCKS = {"ABCD", "ABDC", "ACBD", "ACDB", "ADBC", "ADCB", "BACD", "BADC", "BCAD", "BCDA", "BDAC", "BDCA", "CABD", "CADB", "CBAD", "CBDA", "CDAB", "CDBA", "DABC", "DACB", "DBAC", "DBCA", "DCAB", "DCBA"}

function GetGameInfo()
  -- to do: set the global OFFs and LOCs to their corresponding values
  version = memory.readdword(0x023FFE0C)
  id = bit.band(version, 0xFF)
  language = bit.band(bit.rshift(version, 24), 0xFF)
  print(id, language)
  base=12
  rng=15
  if id == 0x41 then -- Diamond and Pearl
    if lang == 0x44 then base = 0x02107100 rng = 0x0 -- german
    elseif lang == 0x45 then base = 0x02106FC0 rng = 0x021BFB14 -- english
    elseif lang == 0x46 then base = 0x02107140 rng = 0x0 -- french
    elseif lang == 0x49 then base = 0x021070A0 rng = 0x0 -- italian
    elseif lang == 0x4A then base = 0x02108818 rng = 0x0 -- japanese
    elseif lang == 0x4B then base = 0x021045C0 rng = 0x0 -- korean
    elseif lang == 0x53 then base = 0x02107160 rng = 0x0 -- spanish
    end
  elseif id == 0x43 then -- Platinum
    if lang == 0x44 then base = 0x02101EE0 rng = 0x0 -- german
    elseif lang == 0x45 then base = 0x02101D40 rng = 0x021BFB14-- english
    elseif lang == 0x46 then base = 0x02101F20 rng = 0x0 -- french
    elseif lang == 0x49 then base = 0x02101EA0 rng = 0x0 -- italian
    elseif lang == 0x4A then base = 0x02101140 rng = 0x0 -- japanese
    elseif lang == 0x4B then base = 0x02102C40 rng = 0x0 -- korean
    elseif lang == 0x53 then base = 0x02101F40 rng = 0x0 -- spanish
    end
  else print("Invalid game!")
  end
  print(""..string.format("%.8X",base))
  return base, rng
end

-- function GetLanguageIndex(string)
--   for i, v in pairs(LANGUAGES) do
--     if v == string then
--       return i
--     elseif v == "_9" then
--       return 0
--     end
--   end
-- end

function Mult32(a, b)
 c = bit.rshift(a, 16)
 d = a % 0x10000
 e = bit.rshift(b, 16)
 f = b % 0x10000
 g = (c*f + d*e) % 0x10000
 h = d*f
 i = g * 0x10000 + h
 return i
end

function GetBits(a, b, d)
	return bit.rshift(a, b) % bit.lshift(1, d)
end

function GetBlockOrder(pid)
  id = (bit.band(pid, 0x3E000) / 8192) % 24
  return BLOCKS[id+1]
end

function GetSeedDistance(a)
	test = user_seed
  distseed = 0
  for j=0,31,1 do
		if GetBits(a, j, 1) ~= GetBits(test, j, 1) then
			test = Mult32(test, MULTSPA[j+1]) + MULTSPB[j+1]
			distseed = distseed + bit.lshift(1,j)
			if j == 31 then
				distseed = distseed + 0x100000000
			end
		end
  end
	if distseed > 0x7FFFFFFF then
		distseed = distseed - 0xFFFFFFFF
	end
	return distseed
end

function GetRng()
	seed = memory.readdword(rng)
  return seed
end

function SetRng()
	memory.writedword(rng, bit.band(user_seed))
	print("Seed is now "..string.format("%.8X", user_seed))
end

function RngBackwards(f)
  for i=1, f, 1 do
    state = memory.readdword(rng)
    state = Mult32(state, 0xEEB9EB65) + 0xA3561A1
    memory.writedword(rng, bit.band(state))
  end
end

function RngForwards(f)
  for i=1, f, 1 do
    state = memory.readdword(rng)
    state = Mult32(state, 0x41C64E6D) + 0x6073
    memory.writedword(rng, bit.band(state))
  end
end

function GetPointer()
	return memory.readdword(base)
end

function SearchPointer()
	p = user_pointer
	if p ~= 0 and pointer ~= p and pointer ~= 0 and pointer ~= 0xE51C1008 then
		emu.reset()
		print("Searching for pointer "..string.format("%.8X", user_pointer))
	end
end

function GetTid()
  tid = memory.readword(tid_loc)
  sid = memory.readword(tid_loc + 2)
  gui.text(188, -190, "TID = "..string.format("%.5d", tid))
  gui.text(188, -180, "SID = "..string.format("%.5d", sid))
end

function SetTid()
	memory.writeword(tid_loc, user_tid)
  tid = memory.readword(tid_loc)
	memory.writeword(tid_loc+2, user_sid)
  sid = memory.readword(tid_loc + 2)
	print("TID is now "..tid)
end

function GetWildData()
  gui.box(8, 8, 248, 184, "#00000088", "black")
	for i=0,234, 2 do
		x = (i%16)*CHAR_W
		y = math.floor(i/16)*CHAR_H
    v = memory.readword(pointer+WILD_OFF+i)
    if i < 4 then
      color = "green"
    elseif i < 8 then
      color = "yellow"
    elseif i%32 == 8 and i < 106 then
      color = "cyan"
    elseif i == 140 then
      color = "orange"
    else
      color = "white"
    end
		gui.text(17+x, 17+y, string.format("%.4X", v), color)
	end
  pid = memory.readdword(pointer+WILD_OFF)
  bef = memory.readword(pointer+WILD_OFF+4)
  b = GetBlockOrder(pid)
  if bit.band(bef, 4) == 4 then
    gui.text(17+CHAR_W*14, 17+CHAR_H*14, "BEGG", "red")
  else
    gui.text(17+CHAR_W*14, 17+CHAR_H*14, "PKMN", "green")
  end
  gui.text(17+CHAR_W*12, 17+CHAR_H*14, b, "red")
end

function DecryptDump()
	pid = memory.readdword(pointer + OG_WILD_OFF )
	blockValue = (bit.band(pid, 0x3E000)/8192)%24
	badeggflag = memory.readword(pointer + OG_WILD_OFF + 4)
	bef = bit.band(badeggflag, 4)
	chs_1 = bit.band(badeggflag, 1)
	chs_2 = bit.band(badeggflag, 2)
	checksum = memory.readword(pointer + OG_WILD_OFF + 6) -- +6 to get the checksum
	prng = Mult32(checksum, 0x41C64E6D) + 0x6073
	if tabl["4"] then
		print("")
    for i=0, 0x3F do
			offset = i*2
			omod = offset%32
			odiv = math.floor(i/16)-4
			block_id = string.sub(BLOCKS[blockValue+1], odiv, odiv)
			v_encrypted = memory.readword(pointer + OG_WILD_OFF + 8 + offset)
			v_decrypted = bit.bxor(v_encrypted, bit.rshift(prng, 16))
			print(block_id.."_"..string.format("%.2X", omod).." = "..string.format("%.4X", v_decrypted))
			if (offset+2)%32==0 then
				print("")
			end
			prng = Mult32(prng, 0x41C64E6D) + 0x6073
    end
		print("PID = "..string.format("%.8X", pid))
		print("Block order = "..BLOCKS[blockValue+1])
		print("Checksum = "..string.format("%.4X", checksum))
		if bef == 4 then
			print("Bad Egg")
		else
			print("Regular Pkmn")
		end
		if chs_1 == 1 and chs_2 == 2 then
			print("Skipped checksum")
		else
			print("Verified checksum")
		end
		print("Flag = "..string.format("%.4X", badeggflag))
  end
end

function UpdateTab()
  tabl = input.get()
  if tabl["1"] and not prev["1"] then
    RngBackwards(1)
  end
  if tabl["2"] and not prev["2"] then
    RngForwards(1)
	end
  if tabl["8"] and not prev["8"] then
    RngBackwards(16)
  end
  if tabl["9"] and not prev["9"] then
    RngForwards(16)
  end
	if tabl["3"] and not prev["3"] then
    SetTid()
	end
  if tabl["5"] and not prev["5"] then
    SetRng()
	end
  prev=tabl
end


function Main()
  base, rng = GetGameInfo()
  pointer = GetPointer()
  tid_loc = pointer + TID_OFF
  UpdateTab()
	SearchPointer()
	frame = GetRng()
  frame_dist = GetSeedDistance(frame)
  GetTid()
  GetWildData()
  DecryptDump()
	gui.text(2, -190, "Base = "..string.format("%.8X",pointer))
  gui.text(2, -180, "Seed = "..string.format("%.8X",user_seed))
  gui.text(2, -170, "Dist = "..frame_dist)
end

gui.register(Main)
