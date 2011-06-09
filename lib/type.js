var typeHash = {
  UNDEFINED:              255,
  UINT16:                 1,
  INT16:                  2,
  UINT32:                 3,
  INT32:                  4,
  STRING:                 5,
  IPADDR:                 6,
  INT64:                  7,
  UINT64:                 8,
  BOOLEAN:                9,
  BYTE:                   10,
  FLOAT:                  11,
  DOUBLE:                 12,
  IPV4:                   13,
  EVENT:                  15,
  UINT16_ARRAY:           129,
  INT16_ARRAY:            130,
  UINT32_ARRAY:           131,
  INT32_ARRAY:            132,
  STRING_ARRAY:           133,
  IP_ADDR_ARRAY:          134,
  INT64_ARRAY:            135,
  UINT64_ARRAY:           136,
  BOOLEAN_ARRAY:          137,
  BYTE_ARRAY:             138,
  FLOAT_ARRAY:            139,
  DOUBLE_ARRAY:           140,
  IPV4_ARRAY:             141,
  EVENT_ARRAY:            142
};

var typeArray = [];

for(var i=0; i < 256; i++) {
  typeArray[i] = "UNDEFINED";
}

for(type in typeHash) {
  typeArray[typeHash[type]] = type;
}

exports.idToName = function(typeId) {
  return typeArray[typeId];
};
  
exports.nameToId = function(typeName) {
  return typeHash[typeName];
};
