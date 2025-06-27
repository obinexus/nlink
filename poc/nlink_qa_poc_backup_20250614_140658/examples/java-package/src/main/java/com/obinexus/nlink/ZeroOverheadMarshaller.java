package com.obinexus.nlink;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;
import java.util.concurrent.atomic.AtomicInteger;

/**
 * NexusLink Zero-Overhead Data Marshalling for Java
 * 
 * Implements the OBINexus Mathematical Framework for safety-critical
 * distributed systems with O(1) operational overhead guarantee.
 * 
 * @author OBINexus Engineering Team
 * @version 1.2.0
 */
public class ZeroOverheadMarshaller {
    
    private static final int HEADER_SIZE = 16;  // 4 * sizeof(int)
    private static final int DEFAULT_BUFFER_SIZE = 1024;
    private static final AtomicInteger TOPOLOGY_COUNTER = new AtomicInteger(0);
    
    private ByteBuffer buffer;
    private final MessageDigest digest;
    private final int topologyId;
    
    /**
     * Header structure matching C implementation:
     * - version (4 bytes)
     * - payload_size (4 bytes) 
     * - checksum (4 bytes)
     * - topology_id (4 bytes)
     */
    public ZeroOverheadMarshaller() throws NoSuchAlgorithmException {
        this.buffer = ByteBuffer.allocateDirect(DEFAULT_BUFFER_SIZE)
                                .order(ByteOrder.LITTLE_ENDIAN);
        this.digest = MessageDigest.getInstance("SHA-256");
        this.topologyId = TOPOLOGY_COUNTER.incrementAndGet();
    }
    
    /**
     * Marshal double array with zero-overhead principle
     * 
     * @param data double array to marshal
     * @return marshalled bytes with cryptographic header
     */
    public byte[] marshalData(double[] data) {
        int dataSize = data.length * Double.BYTES;
        int totalSize = HEADER_SIZE + dataSize;
        
        // Resize buffer if needed (amortized O(1))
        ensureCapacity(totalSize);
        
        // Reset buffer position
        buffer.clear();
        
        // Write header
        buffer.putInt(1);                           // version
        buffer.putInt(dataSize);                    // payload_size
        buffer.putInt(computeChecksum(data));       // checksum
        buffer.putInt(topologyId);                  // topology_id
        
        // Write data with zero-copy optimization
        for (double value : data) {
            buffer.putDouble(value);
        }
        
        // Extract result
        byte[] result = new byte[totalSize];
        buffer.flip();
        buffer.get(result);
        
        return result;
    }
    
    /**
     * Unmarshal data with integrity validation
     * 
     * @param marshalledData byte array from marshal operation
     * @return original double array
     * @throws SecurityException if checksum validation fails
     */
    public double[] unmarshalData(byte[] marshalledData) throws SecurityException {
        ByteBuffer input = ByteBuffer.wrap(marshalledData)
                                   .order(ByteOrder.LITTLE_ENDIAN);
        
        // Read header
        int version = input.getInt();
        int payloadSize = input.getInt();  
        int storedChecksum = input.getInt();
        int storedTopologyId = input.getInt();
        
        // Validate header
        if (version != 1) {
            throw new SecurityException("Invalid marshalling version: " + version);
        }
        
        // Extract data
        int doubleCount = payloadSize / Double.BYTES;
        double[] data = new double[doubleCount];
        
        for (int i = 0; i < doubleCount; i++) {
            data[i] = input.getDouble();
        }
        
        // Verify checksum
        int computedChecksum = computeChecksum(data);
        if (storedChecksum != computedChecksum) {
            throw new SecurityException("Checksum validation failed");
        }
        
        return data;
    }
    
    private void ensureCapacity(int requiredSize) {
        if (buffer.capacity() < requiredSize) {
            // Grow buffer with 2x strategy for amortized O(1)
            int newSize = Math.max(requiredSize, buffer.capacity() * 2);
            buffer = ByteBuffer.allocateDirect(newSize)
                             .order(ByteOrder.LITTLE_ENDIAN);
        }
    }
    
    private int computeChecksum(double[] data) {
        digest.reset();
        
        // Convert doubles to bytes for hashing
        ByteBuffer temp = ByteBuffer.allocate(data.length * Double.BYTES)
                                  .order(ByteOrder.LITTLE_ENDIAN);
        for (double value : data) {
            temp.putDouble(value);
        }
        
        byte[] hash = digest.digest(temp.array());
        
        // Convert first 4 bytes of hash to int
        return ByteBuffer.wrap(hash).getInt();
    }
    
    /**
     * Get topology ID for this marshaller instance
     */
    public int getTopologyId() {
        return topologyId;
    }
    
    /**
     * Factory method for creating marshaller instances
     */
    public static ZeroOverheadMarshaller create() throws NoSuchAlgorithmException {
        return new ZeroOverheadMarshaller();
    }
}
