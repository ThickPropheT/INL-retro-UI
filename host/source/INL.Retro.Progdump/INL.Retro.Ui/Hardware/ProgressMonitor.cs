using System;
using System.Linq;

namespace INL.Retro.Ui.Hardware
{
    public interface IReportProgress
    {
        int Total { get; }

        void Start(Action<int> progressReport);
    }

    public interface IProgressMonitor
    {
        void StartAndMonitor(Action<int, int> progressReport, params IReportProgress[] operations);
    }

    public class ProgressMonitor : IProgressMonitor
    {
        public void StartAndMonitor(Action<int, int> progressReport, params IReportProgress[] operations)
        {
            var current = 0;
            var total = operations.Sum(o => o.Total);

            foreach (var operation in operations)
            {
                operation.Start(c => progressReport(current + c, total));
                current = operation.Total;
            }
        }
    }
}
