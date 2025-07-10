import com.obinexus.nlink.ZeroOverheadMarshaller;
import java.io.FileOutputStream;
import java.io.IOException;
public class TestMarshaller {
    public static void main(String[] args) {
        try {
            ZeroOverheadMarshaller marshaller = ZeroOverheadMarshaller.create();
            double[] testData = {1.0, 2.5, 3.14159, -4.7};
            byte[] marshalled = marshaller.marshalData(testData);
            
            FileOutputStream fos = new FileOutputStream("/tmp/java_python_test.bin");
            fos.write(marshalled);
            fos.close();
            
            System.out.println("SUCCESS: Java marshalling completed");
        } catch (Exception e) {
            System.err.println("ERROR: " + e.getMessage());
            System.exit(1);
        }
    }
}
