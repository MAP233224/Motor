--------------------------------
--      Motor.lua by MAP      --
--------------------------------

--------------------------------

user_seed = 0x741DCC1F  --enter the seed you got from Motor.exe, press '5' to set it
user_pointer = 0 --automatically search for a specific ASLR
user_tid = 34033 --enter a custom TID, press '3' to set it
user_sid = 60800 --enter a custom SID, press '3' to set it

--------------------------------

mode=1 --switch between Decrypted Battle Data and Encrypted Wild Pokémon Data
MODE_MAX = 3
MODES = {"Encrypted Wild Data", "Decrypted Battle Data", "Nothing"}
tabl = {}
-- {dp, plat} --
RNG_OFF = {0xBDD88, 0xBDDD4}
TID_OFF = {0x288, 0x8C}
PARTY_OFF = {0x2AC, 0xB4}
PKMN_BD_OFF = {0x48580, 0x475B8} -- +0xC0 to get the opponent
WILD_OFF = {0x4CD88, 0x4BE5C}
KOREAN_OFF = 0x44

BASES = {{0x02107100, 0x02101EE0}, {0x02106FC0, 0x02101D40}, {0x02107140, 0x02101F20}, {0x021070A0, 0x02101EA0}, {0x02108818, 0x02101140}, {0x021045C0, 0x02102C40}, {0x02107160, 0x02101F40}}
ZERO_ASLR = {{0x0226D4A0, 0}, {0x0226D260, 0}, {0x0226D5E0, 0}, {0x0226D440, 0}, {0x02271940, 0}, {0x02274B00, 0}, {0x0226d600, 0}}
-- ge, en, fr, it, jp, ko, sp (need to change it later to conventional order)

PKMN_SIZE = 0xEC

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

CLIENT_NAME = {"Trainer", "Opponent"}

LANG_ID = {0x44, 0x45, 0x46, 0x49, 0x4A, 0x4B, 0x53}

function DumpItemIndex()
	local file = io.open("ii_pearl_eng", "wb")
	local b = 0 --byte to be written to file
	for i=0,65536,1 do
		local n = i%8
		local e = memory.readword(0x020F85B4+i*8)
		if e>0x92C3 then
			-- file:write("0x"..string.format("%.4X", i)..": 0x"..string.format("%.4X", e).."\n")
			b = bit.bor(b,bit.lshift(1, n))
		else
			b = bit.bor(b,bit.lshift(0, n))
		end
		if n==7 then
			file:write(string.char(b))
			b = 0
		end
	end
	io.close(file)
	print("Dump successful!")
end

function IndexOf(array, value)
	local index={}
	for k,v in pairs(array) do index[v]=k end
	return index[value]
end

function GetGameInfo()
	-- to do: set the global OFFs and LOCs to their corresponding values
	local vers = memory.readdword(0x023FFE0C)
	local id = bit.band(vers, 0xFF)
	local lang = bit.band(bit.rshift(vers, 24), 0xFF)
	lang=IndexOf(LANG_ID, lang)
	local b=0
	if id==0x41 then vers=1 -- Diamond & Pearl
	elseif id==0x43 then vers=2 -- Platinum
	else print("Invalid game!") return end
	b = BASES[lang][vers]
	return b, vers, lang
end

function GetPointer()
	return memory.readdword(base)
end

function SearchPointer()
	local p = user_pointer
	if p ~= 0 and pointer ~= p and pointer ~= 0 and pointer ~= 0xE51C1008 then
		emu.reset()
		print("Searching for pointer "..string.format("%.8X", user_pointer))
	end
end

function GetAslrIndex()
	return (pointer - ZERO_ASLR[language][version]) / 4
end

function Mult32(a, b)
	local c = bit.rshift(a, 16)
	local d = a % 0x10000
	local e = bit.rshift(b, 16)
	local f = b % 0x10000
	local g = (c*f + d*e) % 0x10000
	local h = d*f
	local i = g * 0x10000 + h
	return i
end

function GetBits(a, b, d)
	return bit.rshift(a, b) % bit.lshift(1, d)
end

function GetBlockOrder(pid)
	local id = (bit.band(pid, 0x3E000) / 8192) % 24
	return BLOCKS[id+1]
end

function GetSeedDistance(a)
	local test = user_seed
 	local distseed = 0
 	for j=0,31,1 do
		if GetBits(a, j, 1) ~= GetBits(test, j, 1) then
			test = Mult32(test, MULTSPA[j+1]) + MULTSPB[j+1]
			distseed = distseed + bit.lshift(1,j)
			if j==31 then distseed = distseed + 0x100000000 end
		end
  	end
	if distseed>0x7FFFFFFF then distseed = distseed - 0xFFFFFFFF end
	return distseed
end

function GetRng()
	local seed = memory.readdword(rng)
	return seed
end

function SetRng()
	memory.writedword(rng, bit.band(user_seed))
	print("Seed is now "..string.format("%.8X", user_seed))
end

function RngBackwards(f)
	for i=1, f, 1 do
		local state = memory.readdword(rng)
		state = Mult32(state, 0xEEB9EB65) + 0xA3561A1
		memory.writedword(rng, bit.band(state))
	end
end

function RngForwards(f)
  for i=1, f, 1 do
    local state = memory.readdword(rng)
    state = Mult32(state, 0x41C64E6D) + 0x6073
    memory.writedword(rng, bit.band(state))
  end
end

function GetTid()
  local tid = memory.readword(tid_loc)
  local sid = memory.readword(tid_loc + 2)
  gui.text(188, -190, "TID = "..string.format("%.5d", tid))
  gui.text(188, -180, "SID = "..string.format("%.5d", sid))
end

function SetTid()
	memory.writeword(tid_loc, user_tid)
  local tid = memory.readword(tid_loc)
	memory.writeword(tid_loc+2, user_sid)
  local sid = memory.readword(tid_loc + 2)
	print("TID set to "..tid)
  print("SID set to "..sid)
end

function GetWildData()
	for i=0,234, 2 do
		local x = (i%16)*CHAR_W
		local y = math.floor(i/16)*CHAR_H
    local v = memory.readword(wild_loc+i)
    if i<4 then color="green"
    elseif i<8 then color="yellow"
    elseif i%32==8 and i<106 then color="cyan"
    elseif i==140 then color="orange"
    else color="white" end
		gui.text(17+x, 17+y, string.format("%.4X", v), color)
	end
  local pid = memory.readdword(wild_loc)
  local bef = memory.readword(wild_loc+4)
  local b = GetBlockOrder(pid)
  if bit.band(bef, 4)==4 then gui.text(17+CHAR_W*14, 17+CHAR_H*14, "BEGG", "red")
  else gui.text(17+CHAR_W*14, 17+CHAR_H*14, "PKMN", "green") end
  gui.text(17+CHAR_W*12, 17+CHAR_H*14, b, "red")
end

function DecryptDump(pkmn_loc)
	local pid = memory.readdword(pkmn_loc)
	local blockValue = (bit.band(pid, 0x3E000)/8192)%24
	local badeggflag = memory.readword(pkmn_loc + 4)
	local bef = bit.band(badeggflag, 4)
	local chs_1 = bit.band(badeggflag, 1)
	local chs_2 = bit.band(badeggflag, 2)
	local checksum = memory.readword(pkmn_loc + 6) -- +6 to get the checksum
	local prng = Mult32(checksum, 0x41C64E6D) + 0x6073
  print("")
  for i=0, 0x3F do
    local offset = i*2
    local omod = (offset%32)/2
    local odiv = math.floor(i/16)-4
    local block_id = string.sub(BLOCKS[blockValue+1], odiv, odiv)
    local v_encrypted = memory.readword(pkmn_loc + 8 + offset)
    local v_decrypted = bit.bxor(v_encrypted, bit.rshift(prng, 16))
    print(block_id..string.format("[%.2d] = ", omod)..string.format("0x%.4X", v_decrypted))
    if (offset+2)%32==0 then
      print("")
    end
    prng = Mult32(prng, 0x41C64E6D) + 0x6073
  end
  print("PID = "..string.format("%.8X", pid))
  print("Block order = "..BLOCKS[blockValue+1])
  print("Checksum = "..string.format("%.4X", checksum))
  if bef==4 then print("Bad Egg")
  else print("Regular Pkmn")
  end
  if chs_1==1 and chs_2==2 then print("Skipped checksum")
  else print("Verified checksum")
  end
  print("Flag = "..string.format("%.4X", badeggflag))
end

function ChangeMode()
  mode = mode+1
  if mode > MODE_MAX then mode=1 end
  print("Mode "..mode..": "..MODES[mode]) --debug
end

function GetPkmnData()
  if mode==3 then return 0
  else
    gui.box(0, 0, 256, 192, "#00000099", "black")
    if mode==1 then GetWildData()
    elseif mode==2 then GetBattleData()
    else return 0 end
  end
end

function GetBattleData()
  for client=1,2,1 do
  	local range = memory.readbyterange(battle_loc+(client-1)*0xC0, 0x80)
  	for i,v in ipairs(range) do
  		local x = ((i-1)%8)*CHAR_W
  		local y = math.floor((i-1)/8)*CHAR_H
  		if i==1 or i==2 then color="red" --species
  		elseif i==39 or i==40 or i==77 or i==78 or i==81 or i==82 or i==127 or i==128 then color="yellow"
  		elseif i>2 and i<25 then color="yellow"
		elseif i>44 and i<53 then color="yellow"
  		elseif i>84 and i<99 then	color="yellow"
  		elseif i>108 and i<113 then	color="yellow"
		elseif i==54 then color="#44ffbb" -- friendship
  		elseif i==37 or i==38 then color="orange" -- type
  		elseif i==121 then color="cyan" -- item
  		elseif i==122 then color="cyan" -- item
  		elseif i>104 and i<109 then color="green" -- PID
  		elseif i>116 and i<121 then color="magenta" -- TID/SID at overwritten PID + 0x6C
  		else color="white" end
  		gui.text(CHAR_W+x+(client-1)*120, CHAR_H*2+y, string.format("%.2X", v), color)
  	end
  	gui.text(CHAR_W+(client-1)*120, CHAR_H, CLIENT_NAME[client])
  end
end

function CopyPkmn(src, dst)
	local partycount = memory.readbyte(pointer+PARTY_OFF[version]-4)
	if dst == 6 then --special case, the whole party becomes the src pkmn
		memory.writebyte(pointer+PARTY_OFF[version]-4, 6) --set the party size to 6
		for dst=0, 5 do
			for i=0, PKMN_SIZE-1 do
				local data = memory.readbyte(pointer+PARTY_OFF[version]+src*PKMN_SIZE+i)
				memory.writebyte(pointer+PARTY_OFF[version]+dst*PKMN_SIZE+i, data)
			end
		end
	print("Filled the whole party with PKMN #"..src)
	else
		if partycount <= dst then memory.writebyte(pointer+PARTY_OFF[version]-4, dst+1) end --checks if party size is smaller than dst slot
		for i=0, PKMN_SIZE-1 do
			local data = memory.readbyte(pointer+PARTY_OFF[version]+src*PKMN_SIZE+i)
			memory.writebyte(pointer+PARTY_OFF[version]+dst*PKMN_SIZE+i, data)
		end
		print("Copied PKMN #"..src.." into PKMN #"..dst)
	end
end

-- function PrintSizePartyCount()
--   local size = memory.readdword(pointer+WILD_OFF[version]-36)
--   local pcm = memory.readdword(pointer+WILD_OFF[version]-8)
--   local pc = memory.readdword(pointer+WILD_OFF[version]-4)
--   print(string.format("%.8x",size).." "..string.format("%.8x",pcm).." "..string.format("%.8x",pc))
-- end

function Controls()
	tabl = input.get()
	if tabl["0"] and not prev["0"] then ChangeMode() end
	if tabl["1"] and not prev["1"] then RngBackwards(1) end
	if tabl["2"] and not prev["2"] then RngForwards(1) end
	if tabl["3"] and not prev["3"] then SetTid() end
	if tabl["4"] and not prev["4"] then DecryptDump(wild_loc) end
	if tabl["5"] and not prev["5"] then SetRng() end
	-- if tabl["6"] and not prev["6"] then DumpItemIndex() end -- use this as the debug function
	if tabl["7"] and not prev["7"] then CopyPkmn(0, 6) end
	if tabl["8"] and not prev["8"] then RngBackwards(16) end
	if tabl["9"] and not prev["9"] then RngForwards(16) end
	prev=tabl
end

function Main()
	base, version, language = GetGameInfo()
	rng = base + RNG_OFF[version]
	if language == 5 and version == 1 then rng = rng + 8 end --jp dp quirk
	pointer = GetPointer()
	aslr_index = GetAslrIndex()
	tid_loc = pointer + TID_OFF[version]
	wild_loc = pointer + WILD_OFF[version]
	battle_loc = pointer + PKMN_BD_OFF[version]
	if language == 6 then --ko
		battle_loc = battle_loc + KOREAN_OFF
		wild_loc = wild_loc + KOREAN_OFF
	end
	Controls()
	SearchPointer()
	frame = GetRng()
	frame_dist = GetSeedDistance(frame)
	GetTid()
	GetPkmnData()
	gui.text(2, -190, "Base = 0x"..string.format("%.8X",pointer).."("..aslr_index..")")
	gui.text(2, -180, "Seed = 0x"..string.format("%.8X",user_seed))
	gui.text(2, -170, "Dist = "..frame_dist)
end

gui.register(Main)
