/^# Packages using this file: / {
  s/# Packages using this file://
  ta
  :a
  s/ madplay / madplay /
  tb
  s/ $/ madplay /
  :b
  s/^/# Packages using this file:/
}
