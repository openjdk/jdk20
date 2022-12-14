/*
 * Copyright (c) 2014, Oracle and/or its affiliates. All rights reserved.
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * This code is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 only, as
 * published by the Free Software Foundation.
 *
 * This code is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * version 2 for more details (a copy is included in the LICENSE file that
 * accompanied this code).
 *
 * You should have received a copy of the GNU General Public License version
 * 2 along with this work; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Please contact Oracle, 500 Oracle Parkway, Redwood Shores, CA 94065 USA
 * or visit www.oracle.com if you need additional information or have any
 * questions.
 */
package org.openjdk.bench.java.util.stream.ops.ref;

import org.openjdk.jmh.annotations.Benchmark;
import org.openjdk.jmh.annotations.BenchmarkMode;
import org.openjdk.jmh.annotations.Fork;
import org.openjdk.jmh.annotations.Measurement;
import org.openjdk.jmh.annotations.Mode;
import org.openjdk.jmh.annotations.OutputTimeUnit;
import org.openjdk.jmh.annotations.Param;
import org.openjdk.jmh.annotations.Scope;
import org.openjdk.jmh.annotations.Setup;
import org.openjdk.jmh.annotations.State;
import org.openjdk.jmh.annotations.Warmup;

import java.util.concurrent.TimeUnit;
import java.util.function.BinaryOperator;
import java.util.stream.LongStream;

/**
 * Benchmark for reduce()-base operation.
 */
@BenchmarkMode(Mode.Throughput)
@Warmup(iterations = 4, time = 2, timeUnit = TimeUnit.SECONDS)
@Measurement(iterations = 4, time = 2, timeUnit = TimeUnit.SECONDS)
@Fork(value = 3)
@OutputTimeUnit(TimeUnit.SECONDS)
@State(Scope.Thread)
public class ReduceBase {

    /**
     * Implementation notes:
     *   - parallel version requires thread-safe sink, we use the same for sequential version for better comparison
     *   - operations are explicit inner classes to untangle unwanted lambda effects
     *   - the result of applying consecutive operations is the same, in order to have the same number of elements in sink
     */

    @Param("100000")
    private int size;

    private BinaryOperator<Long> op1, op2, op3;

    @Setup
    public void setup() {
        op1 = new BinaryOperator<Long>() {
            @Override
            public Long apply(Long l, Long r) {
                return (l > r) ? l : r;
            }
        };
        op2 = new BinaryOperator<Long>() {
            @Override
            public Long apply(Long l, Long r) {
                return (l > r) ? l : r;
            }
        };
        op3 = new BinaryOperator<Long>() {
            @Override
            public Long apply(Long l, Long r) {
                return (l > r) ? l : r;
            }
        };
    }

    @Benchmark
    public long seq_invoke() {
        return LongStream.range(0, size).boxed().reduce(0L, op1);
    }

    @Benchmark
    public long seq_chain_111() {
        long l = 0;
        l += LongStream.range(0, size).boxed().reduce(0L, op1);
        l += LongStream.range(0, size).boxed().reduce(0L, op1);
        l += LongStream.range(0, size).boxed().reduce(0L, op1);
        return l;
    }

    @Benchmark
    public long seq_chain_123() {
        long l = 0;
        l += LongStream.range(0, size).boxed().reduce(0L, op1);
        l += LongStream.range(0, size).boxed().reduce(0L, op2);
        l += LongStream.range(0, size).boxed().reduce(0L, op3);
        return l;
    }

    @Benchmark
    public long par_invoke() {
        return LongStream.range(0, size).parallel().boxed().reduce(0L, op1);
    }

    @Benchmark
    public long par_chain_111() {
        long l = 0;
        l += LongStream.range(0, size).parallel().boxed().reduce(0L, op1);
        l += LongStream.range(0, size).parallel().boxed().reduce(0L, op1);
        l += LongStream.range(0, size).parallel().boxed().reduce(0L, op1);
        return l;
    }

    @Benchmark
    public long par_chain_123() {
        long l = 0;
        l += LongStream.range(0, size).parallel().boxed().reduce(0L, op1);
        l += LongStream.range(0, size).parallel().boxed().reduce(0L, op2);
        l += LongStream.range(0, size).parallel().boxed().reduce(0L, op3);
        return l;
    }

}
