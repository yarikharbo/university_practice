#include <QtTest>
#include <cmath>

static double sinhFunc(double x) { return std::sinh(x); }
static double coshFunc(double x) { return std::cosh(x); }
static double tanhFunc(double x) { return std::tanh(x); }

class TestHyperbolic : public QObject
{
    Q_OBJECT
private slots:
    void testSinh_data();
    void testSinh();
    void testCosh();
    void testTanh();
    void testCoordinateMapping();
};

void TestHyperbolic::testSinh_data()
{
    QTest::addColumn<double>("x");
    QTest::addColumn<double>("expected");
    QTest::newRow("sinh(0)") << 0.0 << 0.0;
    QTest::newRow("sinh(1)") << 1.0 << (std::exp(1.0)-std::exp(-1.0))/2.0;
}

void TestHyperbolic::testSinh()
{
    QFETCH(double, x);
    QFETCH(double, expected);
    QCOMPARE(sinhFunc(x), expected);
}

void TestHyperbolic::testCosh()
{
    QCOMPARE(coshFunc(0.0), 1.0);
    double val = (std::exp(1.0)+std::exp(-1.0))/2.0;
    QVERIFY(std::abs(coshFunc(1.0)-val) < 1e-9);
}

void TestHyperbolic::testTanh()
{
    QCOMPARE(tanhFunc(0.0), 0.0);
    QVERIFY(tanhFunc(100.0) > 0.999);
}

void TestHyperbolic::testCoordinateMapping()
{
    double xMin = 0, xMax = 10, yMin = 0, yMax = 10;
    int plotW = 600, plotH = 400;
    int marginL = 60, marginT = 20;
    double xPan = 0.0, yPan = 0.0;
    double visW = xMax - xMin, visH = yMax - yMin;

    auto worldToPixel = [&](double x, double y) -> QPointF {
        double px = marginL + (x - xPan) / visW * plotW;
        double py = marginT + (1.0 - (y - yPan) / visH) * plotH;
        return QPointF(px, py);
    };

    QPointF p0 = worldToPixel(0.0, 0.0);
    QVERIFY(std::abs(p0.x() - marginL) < 1e-6);
    QVERIFY(std::abs(p0.y() - (marginT + plotH)) < 1e-6);

    QPointF p10 = worldToPixel(10.0, 10.0);
    QVERIFY(std::abs(p10.x() - (marginL + plotW)) < 1e-6);
    QVERIFY(std::abs(p10.y() - marginT) < 1e-6);
}

QTEST_MAIN(TestHyperbolic)
#include "tst_plot.moc"