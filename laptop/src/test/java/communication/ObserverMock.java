package communication;

import model.Data;

import java.util.Observable;
import java.util.Observer;

public class ObserverMock implements Observer {

    Data received;

    @Override
    public void update(Observable o, Object arg) {
        if (arg instanceof Data) {
            received = (Data) arg;
        }
    }
}
