using Mmap
using Match
using BenchmarkTools
using Profile
using Distributed
Threads.nthreads()

function readtoend()
    res = UInt64(0)
    f = open("./COX3.txt", "r")
    line = ""
    while !eof(f)
        line = readline(f)
#         res += sum(line)
    end
    line
end

function mmap_readtoend(step::Int)
    res = UInt64(0)
    f = open("./COX3.txt", "r")
    m = Mmap.mmap(f)
    block = zeros(UInt8, step)
    s = 1
    e = 1
    l = length(m)
    while e < l
        e += step
        if e > l
            e = l
        end
        
        res += sum(m[s: e])
        s += step
        
    end
    res
end

function get_mmap()
#     f = open("./COX3.txt", "r")
    f = open("D:/sync/merge_GetExp_gene.txt", "r")
    m = Mmap.mmap(f)
    m
end

function addx(x::UInt, n::UInt8)
    return (x * 10 + n)::UInt
end

function mmap_read_limit(start_pos::Int, end_pos::Int)

    m = get_mmap()
    current_pos = start_pos
    current_status = 1

    l = length(m)
    if end_pos === 0
        end_pos = l
    end

    current_x = UInt(0)
    current_y = UInt(0)
    max_x = max_y = UInt(0)
    min_x = min_y = typemax(UInt)

    while current_pos < end_pos && current_pos < l
        @inbounds current_char = m[current_pos]
        if current_char === UInt8('\t')
            current_status += 1 
        elseif current_char === UInt8('\n')
            current_status = 1
            max_x = max(max_x, current_x)
            max_y = max(max_y, current_y)
            min_x = min(min_x, current_x)
            min_y = min(min_y, current_y)
            current_x = current_y = UInt(0)
    #         println("$max_x, $max_y, $min_x, $min_y\n")
        else
            if current_status === 1 
                current_x = addx(current_x, current_char - UInt8('0'))::UInt
            elseif current_status === 2
                current_y = addx(current_y, current_char - UInt8('0'))::UInt
            end

        end


        current_pos += 1
    #     print("$current_pos, $current_status, $current_x, $current_y, $(Char(current_char))\n")
    #     if current_pos > 30
    #         break
    #     end
    end
    #     print("$current_pos, $current_status, $current_x, $current_y, $(Char(current_char))\n")
    current_pos, max_x, max_y, min_x, min_y
end


function find_line_end_helper(m::Vector{UInt8}, start::Int)
    
    while start <= length(m)
        if m[start] == UInt8('\n')
            break
        else
            start += 1
        end
    end
    
    return start
end


function find_line_end(m::Vector{UInt8}, chunk_size::Int)
    l = length(m)
    positions = Tuple{Int, Int}[]
    start = 1
    
    single_size = floor(Int, l / chunk_size)
    
    for i = 1:chunk_size
        new_start = start + single_size
        if new_start > l
            new_start = l
        end
        
        new_start = find_line_end_helper(m, new_start)
        
        push!(positions, (start, new_start))
        start = new_start + 1
    end
    positions
end
    
function mmap_read_limit_pmap(size::Int)
    m = get_mmap()
    line_ends = find_line_end(m, size)
    pmap(i -> mmap_read_limit(i[1], i[2]), line_ends)
end
# @time mmap_read_limit_pmap(6)
# @time mmap_read_limit_pmap(6)
# @time mmap_read_limit_pmap(6)
# @time mmap_read_limit_pmap(6)

function mmap_read_limit_thr(size::Int)
    m = get_mmap()
    line_ends = find_line_end(m, size)
    Threads.@threads for i = line_ends
        println(mmap_read_limit(i[1], i[2]))
    end
end

@time mmap_read_limit_thr(6)
@time mmap_read_limit_thr(6)
@benchmark mmap_read_limit_thr(6)
