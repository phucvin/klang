local sum = 0
for i = 1, 1000000000 do
  sum = sum + i
end
print(sum)



--f = function(n)
  --return n <= 1 and n or f(n - 1) + f(n - 2)
--end
--print(f(35))