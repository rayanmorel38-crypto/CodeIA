import unittest
from data_analysis import cleaner, transformer, aggregator, statistics

class TestDataAnalysis(unittest.TestCase):

    def setUp(self):
        self.sample_data = [1, 2, 3, 4, 5, None, 7]

    def test_cleaner_removes_none(self):
        cleaned = cleaner.run_task(task={'data': self.sample_data})
        self.assertNotIn(None, cleaned)
        self.assertEqual(len(cleaned), 6)

    def test_transformer_min_max(self):
        data = [1, 2, 3]
        transformed = transformer.run_task(task={'data': data}, data=data, method='min-max')
        self.assertAlmostEqual(transformed[0], 0.0)
        self.assertAlmostEqual(transformed[-1], 1.0)

    def test_transformer_z_score(self):
        data = [1, 2, 3]
        transformed = transformer.run_task(task={'data': data}, data=data, method='z-score')
        self.assertAlmostEqual(sum(transformed), 0.0, delta=1e-6)

    def test_aggregator_metrics(self):
        data = [1, 2, 3, 4]
        metrics = aggregator.run_task(task={'data': data}, data=data)
        self.assertEqual(metrics['sum'], 10)
        self.assertEqual(metrics['count'], 4)
        self.assertEqual(metrics['mean'], 2.5)

    def test_statistics_metrics(self):
        data = [1, 2, 3, 4, 5]
        stats = statistics.run_task(task={'data': data}, data=data)
        self.assertAlmostEqual(stats['mean'], 3.0)
        self.assertAlmostEqual(stats['variance'], 2.0)
        self.assertAlmostEqual(stats['std_dev'], 2.0**0.5)
        self.assertEqual(stats['median'], 3)

if __name__ == '__main__':
    unittest.main()

