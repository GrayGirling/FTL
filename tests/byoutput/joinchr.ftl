# Assumes Display is set to Unicode UTF-8
eval echo (joinchr 167 <"para1", "para2", "para3">!)!
#para1§para2§para3
eval echo (joinchr NULL <915, 961, 945, 953>!)!
#Γραι
set chshow [n]:{ echo (joinchr " " <n .. 28+(n)>!)! }
chshow 913
#Α Β Γ Δ Ε Ζ Η Θ Ι Κ Λ Μ Ν Ξ Ο Π Ρ ΢ Σ Τ Υ Φ Χ Ψ Ω Ϊ Ϋ ά έ 
