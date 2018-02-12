﻿using System;
using System.Collections.Generic;

namespace Log4JParserNet
{
    public sealed class FilterMessage
        : Filter
        , IEquatable<FilterMessage>
    {
        public static readonly IEqualityComparer<string> MessageComparer
            = StringComparer.Ordinal;

        public const StringComparison MessageComparison
            = StringComparison.Ordinal;

        new public string Message { get; }

        public FilterMessage (string message)
        {
            Message = message;
        }

        public override bool Equals (object obj)
            => obj is FilterMessage other && Equals (other);

        public bool Equals (FilterMessage other)
            => other != null && MessageComparer.Equals (Message, other.Message);

        public override int GetHashCode ()
            => 2114237065 + MessageComparer.GetHashCode (Message);

        internal override HandleGraph<FilterHandle> Build ()
        {
            FilterHandle result = null;
            try
            {
                Log4JParserC.Log4JFilterInitMessageNt (out result, Message);
                return HandleGraph.Simple (result);
            }
            catch (Exception ex)
            {
                Disposable.DisposeAggregateErrors (result, ex);
                throw;
            }
        }

        public override void AcceptVisitor (IFilterVisitor visitor)
            => (visitor ?? throw new ArgumentNullException (nameof (visitor))).Visit (this);
    }
}