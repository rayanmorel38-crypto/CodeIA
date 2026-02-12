import unittest
from ml import prediction, anomaly_detection

class TestML(unittest.TestCase):

    def setUp(self):
        self.sample_data = [1, 2, 3, 4, 5]

    def test_prediction_with_model(self):
        def model(x):
            return x * 2
        result = prediction.run_task(task={'data': self.sample_data}, model=model, data=self.sample_data)
        self.assertEqual(result, [2, 4, 6, 8, 10])

    def test_prediction_without_model(self):
        result = prediction.run_task(task={'data': self.sample_data}, data=self.sample_data)
        self.assertEqual(result, self.sample_data)

    def test_anomaly_detection_default_threshold(self):
        data = [10, 10, 10, 100]
        anomalies = anomaly_detection.run_task(task={'data': data}, data=data, use_std_threshold=True)
        self.assertEqual(anomalies, [100])

    def test_anomaly_detection_custom_threshold(self):
        data = [1, 2, 3, 10]
        anomalies = anomaly_detection.run_task(task={'data': data}, data=data, threshold=5)
        self.assertEqual(anomalies, [10])

if __name__ == '__main__':
    unittest.main()

